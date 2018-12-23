#include "Frame.h"
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <vector>  //for std::vector

using namespace std;
using namespace cv;

Frame::Frame(Mat &img) : image(img), sumOfSquares(Mat::zeros(image.rows, image.cols, CV_32SC1)) {
    fillSum();
}

void Frame::fillSum() {
    CV_Assert(sumOfSquares.size() == image.size()); // sums should be stored in the same size mat


    int current_line_sum = 0;

    // fill the first row of sum matrix
    for (int j = 0; j < image.cols; j++) {
        current_line_sum += image.at<uchar>(0, j);

        sumOfSquares.at<uint>(0, j) = static_cast<uint>(current_line_sum);
    }

    // fill other rows, using sum of previous rows
    for (int i = 1; i < image.rows; i++) {
        current_line_sum = 0;

        for (int j = 0; j < image.cols; j++) {
            current_line_sum += image.at<uchar>(i, j);

            sumOfSquares.at<uint>(i, j) = current_line_sum + sumOfSquares.at<uint>(i - 1, j);
        }
    }
}


int Frame::rectSum(int x, int y, int width, int height) {
    if (x == 0 && y == 0)
        return sumOfSquares.at<uint>(height - 1, width - 1);
    if (x == 0)
        return sumOfSquares.at<uint>(y + height - 1, width - 1) - sumOfSquares.at<uint>(y - 1, width - 1);
    if (y == 0)
        return sumOfSquares.at<uint>(height - 1, x + width - 1) - sumOfSquares.at<uint>(height - 1, x - 1);

    return sumOfSquares.at<uint>(y + height - 1, x + width - 1) - sumOfSquares.at<uint>(y + width - 1, x - 1) - sumOfSquares.at<uint>(y - 1, x + width - 1) + sumOfSquares.at<uint>(y - 1, x - 1);

}

int saturation(int a) {
    if (a > 255) return 255;
    if (a < 0) return 0;
    return a;
}


Mat &Frame::blobConv(Mat &result) {
    //  (-1, -1, -1, -1, -1,
    //   -1,  1,  1,  1, -1,
    //   -1,  1,  8,  1, -1,
    //   -1,  1,  1,  1, -1,
    //   -1, -1, -1, -1, -1 )
    Mat_<uchar> result_ = result;
    int n = 2;
    for (int i = n; i < image.rows - n; i++) {
        for (int j = n; j < image.cols - n; j++) {
            // todo: check how to convert to uchar without saturation..
            result_[i][j] = saturation(-rectSum(j - n, i - n, 2 * n + 1, 2 * n + 1) + 2 * rectSum(j - n + 1, i - n + 1, 2 * n - 1, 2 * n - 1) + 7 * rectSum(j, i, 1, 1));
        }
    }
    result = result_;
    return result;
}

Mat &Frame::cornerConv(Mat &result) {
    //  (-1, -1,  0,  1,  1,
    //   -1, -1,  0,  1,  1,
    //    0,  0,  0,  0,  0,
    //    1,  1,  0, -1, -1,
    //    1,  1,  0, -1, -1 )

    Mat_<uchar> result_ = result;
    int n = 2;
    for (int i = n; i < image.rows - n; i++ ) {
        for (int j = n; j < image.cols - n; j++) {
            result_[i][j] = saturation(-rectSum(j - n, i - n, n, n) + rectSum(j - n, i, n, n) + rectSum(j, i - n, n, n) - rectSum(j + 1, i + 1, n, n));
        }
    }
    result = result_;
    return result;
}


int Frame::compPartialMax(int from, int to, int *I, int *pmax) {
    pmax[to] = I[to];
    int best = to;
    while (to > from) {
        to--;
        // m not sure, should i compare all three intensiveness or what
        if (I[best] >= I[to]) {
            pmax[to] = I[best];
        } else {
            pmax[to] = I[to];
            best = to;
        }
    }
    return best;
}


void Frame::suppression1D(int n, int *I, int size, vector<int> &maximum) {
    int pmax[size];
    int i = n;
    compPartialMax(0, i - 1, I, pmax);
    int chkpt = -1;
    while (i < size - 2*n) {
        cout << "hello" << endl;
        int j = compPartialMax(i, i + n, I, pmax);
        int k = compPartialMax(i + n + 1, j + n, I, pmax);
        if (i == j || I[j] >= I[k]) {
            if ((chkpt <= j - n || I[j] >= pmax[chkpt]) &&
                (j - n == i || I[j] >= pmax[j - n])) {
                maximum.push_back(j);
            }
            if (i < j) {
                chkpt = i + n + 1;
            }
            i = j + n + 1;
        }
        else {
            i = k;
            chkpt = j + n + 1;
            while (i < size - n) {
                j = compPartialMax(chkpt, i + n, I, pmax);
                if (I[i] >= I[j]) {
                    maximum.push_back(i);
                    i = i + n - 1;
                    break;
                }
                else {
                    chkpt = i + n - 1;
                    i = j;
                }
            }
        }
    }
}

bool cmp(uchar a, uchar b, int cmp) {
    return (cmp == 1) ? a > b : b > a;
}


// founded on (2n + 1) × (2n + 1)-Block Algorithm from "Efficient Non-Maximum Suppression", Alexander Neubeck, Luc Van Gool;
// if comp == 1, then it found local maximum, else - local minimum
void Frame::suppression2D(int n, uchar **I, int H, int W, vector<pair<int, int>> &maximum, int comp) {
    cout << H << " " << W << endl;
    for (int i = n; i <= H - n; i += n + 1) {
        for (int j = n; j <= W - n; j += n + 1) {
            cout << i << " " << j << endl;

            int mi = i;
            int mj = j;
            for (int i2 = i; i2 <= i + n; i2++) {
                for (int j2 = j; j2 <= j + n; j2++) {
                    if (cmp(I[i2][j2], I[mi][mj], comp)) {
                        mi = i2;
                        mj = j2;
                    }
                }
            }
            int flag = 0;
            for (int i2 = mi - n; i2 <= mi + n && i2 < H; i2++) {
                for (int j2 = mj - n; j2 <= j && j2 < W; j2++) {
                    if (cmp(I[i2][j2], I[mi][mj], comp)) {
                        flag = 1;
                        break;
                    }
                }
                if (flag == 1) break;
                for (int j2 = j + n; j2 <= mj + n && j2 < W; j2++) {
                    if (cmp(I[i2][j2], I[mi][mj], comp)) {
                        flag = 1;
                        break;
                    }
                }
                if (flag == 1) break;
            }
            if (flag == 0) {
                for (int j2 = j; j2 <= j + n && j2 < W; j2++) {
                    for (int i2 = mi - n; i2 <= i && i2 < H; i2++) {
                        if (cmp(I[i2][j2], I[mi][mj], comp)) {
                            flag = 1;
                            break;
                        }
                    }
                    if (flag == 1) break;
                    for (int i2 = i + n; i2 <= mi + n && i2 < H; i2++) {
                        if (cmp(I[i2][j2], I[mi][mj], comp)) {
                            flag = 1;
                            break;
                        }
                    }
                    if (flag == 1) break;
                }
            }
            if (flag == 0) {
                maximum.emplace_back(pair<int, int>(mi, mj));
            }

        }
    }

}



