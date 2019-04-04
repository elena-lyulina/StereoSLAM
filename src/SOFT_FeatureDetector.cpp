#include <opencv/cv.hpp>
#include "SOFT_FeatureDetector.h"



SOFT_FeatureDetector::SOFT_FeatureDetector(pair<Frame, Frame> &prevImages, pair<Frame, Frame> &succImages) :
    prevImages(prevImages), succImages(succImages) {

  const int imgAmount = 4;
  const int fpTypesAmount = 4;
  const int convTypesAmount = 2;
  int supprArea = 10;
  Frame *frames[imgAmount] = {&prevImages.first, &prevImages.second, &succImages.first, &succImages.second};

  vector<pair<int, int>> coord[imgAmount][fpTypesAmount];
  vector<FeaturePoint> fp[imgAmount][fpTypesAmount];
  vector<fp_tuple> *mp[fpTypesAmount] = {&blobMaxMatchingPoints, &blobMinMatchingPoints, &cornerMaxMatchingPoints, &cornerMinMatchingPoints};

  //tuple<greater<int>, less<int>> comp = {greater<int>(), less<int>()};
  for (int i = 0; i < imgAmount; i++) {
    frames[i]->suppression2D(supprArea, frames[i]->getBlobConvolution(), coord[i][0],
                             greater<int>());
    frames[i]->suppression2D(supprArea, frames[i]->getBlobConvolution(), coord[i][1], less<int>());
    frames[i]->suppression2D(supprArea, frames[i]->getCornerConvolution(), coord[i][2],
                             greater<int>());
    frames[i]->suppression2D(supprArea, frames[i]->getCornerConvolution(), coord[i][3],
                             less<int>());

    for (int k = 0; k < fpTypesAmount; k++) {
      for (int j = 0; j < coord[i][k].size(); j++) {
        fp[i][k].emplace_back(FeaturePoint(*frames[i], coord[i][k][j].first, coord[i][k][j].second));
      }
    }
  }

  for (int k = 0; k < fpTypesAmount; k++) {
    doMatchingCircle(fp[0][k], fp[1][k], fp[3][k], fp[2][k], *mp[k]);
  }

}

int SADofDescriptors (const FeaturePoint &fp1, const FeaturePoint &fp2)
{
  const int N = FeaturePoint::matchingDescriptorSize;
  int sad = 0;
  for (int i = 0; i < N; ++i)
    sad += std::abs(fp1.matchingDescriptor[i] - fp2.matchingDescriptor[i]);
  return sad;
}


const FeaturePoint* findMatchesOnArea (const FeaturePoint &p, const vector<FeaturePoint> &candidates, int areaSize)
{
  int minError = SADofDescriptors (p, candidates[0]);
  const FeaturePoint* matchedPoint = &candidates[0];
  for (auto &rsPoint : candidates)
  {
    if (abs (rsPoint.col - p.col) <= areaSize && abs (rsPoint.row - p.row) <= areaSize)
    {

      int error = SADofDescriptors (rsPoint, p);

      if (error < minError)
      {
        minError = error;
        matchedPoint = &rsPoint;
      }
    }
  }

  return matchedPoint;
}

template <typename Comparator>
const FeaturePoint *findMatchesOnStrip (const FeaturePoint &p, const vector<FeaturePoint> &candidates, int stripWidth, Comparator comp)
{
  int minError = SADofDescriptors (p, candidates[0]);
  const FeaturePoint* matchedPoint = &candidates[0];

  for (auto &rsPoint : candidates)
  {
    if (comp (rsPoint.col, p.col) && abs (rsPoint.row - p.row) <= stripWidth)
    {

      int error = SADofDescriptors (rsPoint, p);

      if (error < minError)
      {
        minError = error;
        matchedPoint = &rsPoint;
      }
    }
  }

  return matchedPoint;
}


void SOFT_FeatureDetector::doMatchingCircle (const vector<FeaturePoint> &leftPred,
                                        const vector<FeaturePoint> &rightPred,
                                        const vector<FeaturePoint> &rightSucc,
                                        const vector<FeaturePoint> &leftSucc,
                                        vector<tuple<const FeaturePoint*, const FeaturePoint*, const FeaturePoint*, const FeaturePoint*>> &matched)
{

  int stripWidth = 10;
  int areaWidth = 50;

  for (auto &lpPoint : leftPred)
  {

    // возвращать указатель или id
    // checking strip on right pred image, it should be to the right of lpPoint
    const FeaturePoint* rpMatchedPoint = findMatchesOnStrip (lpPoint, rightPred, stripWidth, std::less<int> ());

    // checking area on right succ image
    const FeaturePoint* rsMatchedPoint = findMatchesOnArea (*rpMatchedPoint, rightSucc, areaWidth);

    // checking strip on left succ image, it should be to the left of rsMatchedPoint
    const FeaturePoint* lsMatchedPoint =
        findMatchesOnStrip (*rsMatchedPoint, leftSucc, stripWidth, std::greater<int> ());

    // checking are on left pred image
    const FeaturePoint* lpMatchedPoint = findMatchesOnArea (*lsMatchedPoint, leftPred, areaWidth);

    if (&lpPoint == lpMatchedPoint)
    {
      matched.emplace_back (lpMatchedPoint, rpMatchedPoint, rsMatchedPoint, lsMatchedPoint);
    }
  }
}
vector<fp_tuple> SOFT_FeatureDetector::getMatchingPoints() {
  return vector<fp_tuple>();
}

vector<fp_tuple> SOFT_FeatureDetector::getBlobMaxMatchingPoints() {
  return blobMaxMatchingPoints;
}

vector<fp_tuple> SOFT_FeatureDetector::getBlobMinMatchingPoints() {
  return blobMinMatchingPoints;
}

vector<fp_tuple> SOFT_FeatureDetector::getCornerMaxMatchingPoints() {
  return cornerMaxMatchingPoints;
}

vector<fp_tuple> SOFT_FeatureDetector::getCornerMinMatchingPoints() {
  return cornerMinMatchingPoints;
}


void SOFT_FeatureDetector::showMP() {
  const int imgAmount = 4;
  Frame *frames[imgAmount] = {&prevImages.first, &prevImages.second, &succImages.first, &succImages.second};

  Mat imagesRGB[imgAmount];
  for (int i = 0; i < imgAmount; i++) {
    imagesRGB[i] = Mat (frames[i]->getImage().rows, frames[i]->getImage().cols, CV_8UC3, Scalar::all (0));
    cvtColor(frames[i]->getImage(), imagesRGB[i], COLOR_GRAY2BGR);
  }

  RNG rng (12345);
  Scalar color = Scalar (rng.uniform (10, 255), rng.uniform (10, 255), rng.uniform (10, 255));
  for (int i = 0; i < blobMaxMatchingPoints.size(); i++) {
    circle (imagesRGB[0], Point (get<0> (blobMaxMatchingPoints[i])->col, get<0> (blobMaxMatchingPoints[i])->row), 3, color, -1);
    circle (imagesRGB[1], Point (get<1> (blobMaxMatchingPoints[i])->col, get<1> (blobMaxMatchingPoints[i])->row), 3, color, -1);
    circle (imagesRGB[2], Point (get<2> (blobMaxMatchingPoints[i])->col, get<2> (blobMaxMatchingPoints[i])->row), 3, color, -1);
    circle (imagesRGB[3], Point (get<3> (blobMaxMatchingPoints[i])->col, get<3> (blobMaxMatchingPoints[i])->row), 3, color, -1);

    color = Scalar(rng.uniform(10 ,255), rng.uniform(10, 255), rng.uniform(10, 255));
  }

  for (int i = 0; i < imgAmount; i++)
    {
        namedWindow ("image" + to_string (i), WINDOW_AUTOSIZE);
        imshow ("image" + to_string (i), imagesRGB[i]);
    }

  waitKey();


}






