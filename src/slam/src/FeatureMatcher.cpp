#include <FeatureMatcher.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <opencv/cv.hpp>


FeatureMatcher::FeatureMatcher (FeatureDetector &fd)
{
    doMatchingCircle (fd.getDetectedPoints (LEFT_PRED, BLOB_MAX), fd.getDetectedPoints (RIGHT_PRED, BLOB_MAX),
                      fd.getDetectedPoints (RIGHT_SUCC, BLOB_MAX),
                      fd.getDetectedPoints (LEFT_SUCC, BLOB_MAX), matchedPoints[BLOB_MAX]);

    doMatchingCircle (fd.getDetectedPoints (LEFT_PRED, BLOB_MIN), fd.getDetectedPoints (RIGHT_PRED, BLOB_MIN),
                      fd.getDetectedPoints (RIGHT_SUCC, BLOB_MIN),
                      fd.getDetectedPoints (LEFT_SUCC, BLOB_MIN), matchedPoints[BLOB_MIN]);

    doMatchingCircle (fd.getDetectedPoints (LEFT_PRED, CORNER_MAX), fd.getDetectedPoints (RIGHT_PRED, CORNER_MAX),
                      fd.getDetectedPoints (RIGHT_SUCC, CORNER_MAX),
                      fd.getDetectedPoints (LEFT_SUCC, CORNER_MAX), matchedPoints[CORNER_MAX]);

    doMatchingCircle (fd.getDetectedPoints (LEFT_PRED, CORNER_MIN), fd.getDetectedPoints (RIGHT_PRED, CORNER_MIN),
                      fd.getDetectedPoints (RIGHT_SUCC, CORNER_MIN),
                      fd.getDetectedPoints (LEFT_SUCC, CORNER_MIN), matchedPoints[CORNER_MIN]);
}


int FeatureMatcher::SAD_32x8 (__m256i a1, __m256i a2)
{
    __m256i sub = _mm256_sub_epi32 (a1, a2);
    __m256i abs = _mm256_abs_epi32 (sub);

    __m256i t1 = _mm256_hadd_epi32 (abs, abs);
    __m256i t2 = _mm256_hadd_epi32 (t1, t2);
    __m128i t3 = _mm256_extractf128_si256 (t2, 1);
    __m128i t4 = _mm_add_epi32 (_mm256_castsi256_si128 (t2), t3);

    return _mm_cvtsi128_si32 (t4);
}


int FeatureMatcher::SAD_32x48 (const int32_t (&arr1)[FeaturePoint::mdSize],
                               const int32_t (&arr2)[FeaturePoint::mdSize])
{
    __m256i a1 = _mm256_load_si256 ((__m256i *)arr1);
    __m256i a2 = _mm256_load_si256 ((__m256i *)arr2);

    __m256i b1 = _mm256_loadu_si256 ((__m256i *)(arr1 + 8));
    __m256i b2 = _mm256_loadu_si256 ((__m256i *)(arr2 + 8));

    __m256i c1 = _mm256_loadu_si256 ((__m256i *)(arr1 + 16));
    __m256i c2 = _mm256_loadu_si256 ((__m256i *)(arr2 + 16));

    __m256i d1 = _mm256_loadu_si256 ((__m256i *)(arr1 + 24));
    __m256i d2 = _mm256_loadu_si256 ((__m256i *)(arr2 + 24));

    __m256i e1 = _mm256_loadu_si256 ((__m256i *)(arr1 + 32));
    __m256i e2 = _mm256_loadu_si256 ((__m256i *)(arr2 + 32));

    __m256i f1 = _mm256_loadu_si256 ((__m256i *)(arr1 + 40));
    __m256i f2 = _mm256_loadu_si256 ((__m256i *)(arr2 + 40));

    return SAD_32x8 (a1, a2) + SAD_32x8 (b1, b2) + SAD_32x8 (c1, c2) + SAD_32x8 (d1, d2) +
           SAD_32x8 (e1, e2) + SAD_32x8 (f1, f2);
}

int FeatureMatcher::SAD_8x48 (uint8_t (&arr1)[FeaturePoint::mdSize], uint8_t (&arr2)[FeaturePoint::mdSize])
{

    // first 32 of 48
    __m256i a1 = _mm256_load_si256 ((__m256i *)arr1);
    __m256i a2 = _mm256_load_si256 ((__m256i *)arr2);
    __m256i sad1 = _mm256_sad_epu8 (a1, a2);

    // remaining 16
    __m128i b1 = _mm_loadu_si128 ((__m128i *)(arr1 + 32));
    __m128i b2 = _mm_loadu_si128 ((__m128i *)(arr2 + 32));
    __m128i sad2 = _mm_sad_epu8 (b1, b2);


    int a = _mm_extract_epi16 (sad2, 0) + _mm_extract_epi16 (sad2, 4) +
            _mm256_extract_epi16 (sad1, 0) + _mm256_extract_epi16 (sad1, 4) +
            _mm256_extract_epi16 (sad1, 8) + _mm256_extract_epi16 (sad1, 12);

    return a;
}

int SADofDescriptors (const FeaturePoint &fp1, const FeaturePoint &fp2)
{
    const int N = FeaturePoint::mdSize;
    int sad = 0;
    for (int i = 0; i < N; ++i)
        sad += std::abs (fp1.matchingDescriptor[i] - fp2.matchingDescriptor[i]);
    return sad;
}


const FeaturePoint *
findMatchesOnArea (const FeaturePoint &p, const std::vector<FeaturePoint> &candidates, const cv::Rect &area)
{
    int minError = FeatureMatcher::SAD_32x48 (p.matchingDescriptor, candidates[0].matchingDescriptor);
    // SADofDescriptors (p, candidates[0]);
    const FeaturePoint *matchedPoint = &candidates[0];
    for (int i = 0; i < candidates.size (); i++)
    {
        if (area.contains (cv::Point (p.col, p.row)))
        {

            int error = FeatureMatcher::SAD_32x48 (candidates[i].matchingDescriptor, p.matchingDescriptor);
            // SADofDescriptors (candidates[i], p);

            if (error < minError)
            {
                minError = error;
                matchedPoint = &candidates[i];
            }
        }
    }

    return matchedPoint;
}

void FeatureMatcher::doMatchingCircle (
const std::vector<FeaturePoint> &leftPred,
const std::vector<FeaturePoint> &rightPred,
const std::vector<FeaturePoint> &rightSucc,
const std::vector<FeaturePoint> &leftSucc,
std::vector<std::tuple<const FeaturePoint *, const FeaturePoint *, const FeaturePoint *, const FeaturePoint *>> &matched)
{

    int stripWidth = 1;
    int areaWidth = 50;

    for (int i = 0; i < leftPred.size (); i++)
    {
        // checking strip on right pred image, it should be to the right of leftPred[i]
        const FeaturePoint *rpMatchedPoint =
        findMatchesOnArea (leftPred[i], rightPred,
                           cv::Rect (leftPred[i].col, leftPred[i].row - stripWidth,
                                     leftPred[i].w - leftPred[i].col, 2 * stripWidth));

        // checking area on right succ image
        const FeaturePoint *rsMatchedPoint =
        findMatchesOnArea (*rpMatchedPoint, rightSucc,
                           cv::Rect (rpMatchedPoint->col - areaWidth,
                                     rpMatchedPoint->row - areaWidth, 2 * areaWidth, 2 * areaWidth));

        // checking strip on left succ image, it should be to the left of rsMatchedPoint
        const FeaturePoint *lsMatchedPoint =
        findMatchesOnArea (*rsMatchedPoint, leftSucc,
                           cv::Rect (rsMatchedPoint->col, rsMatchedPoint->row - stripWidth,
                                     rsMatchedPoint->w - rsMatchedPoint->col, 2 * stripWidth));

        // checking are on left pred image
        const FeaturePoint *lpMatchedPoint =
        findMatchesOnArea (*lsMatchedPoint, leftPred,
                           cv::Rect (lsMatchedPoint->col - areaWidth,
                                     lsMatchedPoint->row - areaWidth, 2 * areaWidth, 2 * areaWidth));

        if (&leftPred[i] == lpMatchedPoint)
        {
            matched.emplace_back (lpMatchedPoint, rpMatchedPoint, rsMatchedPoint, lsMatchedPoint);
        }
    }
}

std::vector<fp_tuple> FeatureMatcher::getMatchedPoints (pointType type)
{
    return matchedPoints[type];
}
std::vector<fp_tuple> *FeatureMatcher::getMatchedPointsAllTypes ()
{
    return matchedPoints;
}
