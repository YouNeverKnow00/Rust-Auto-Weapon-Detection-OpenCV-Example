#include <iostream>
#include <Windows.h>
#include <WinUser.h>
#include <opencv2/core/core.hpp>
#include "opencv2/core.hpp"

#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

#include "opencv2/video/background_segm.hpp"

//
bool test;

using namespace cv;
std::vector<Scalar> rgbValues = {
    Scalar(114,98,115),
    Scalar(112,70,140),
    Scalar(70, 70, 180),
    Scalar(117, 30, 140),
    Scalar(112, 30, 120),
};
struct RGB {
    int r = 0;
    int g = 0;
    int b = 0;
}rgb;

void filter_RGB_VALUES(std::vector<Scalar> rgbValues) {
    std::cout << rgbValues.size();
    for (int i = 0; i < rgbValues.size(); i++)
    {
        if (rgbValues[i][2] > 180 || rgbValues[i][2] < 120) {
            rgbValues.erase(rgbValues.begin() + i);
        }
    }
    std::cout << rgbValues.size() << "\n";
    std::vector<RGB> rgb_Averages;
    for (int i = 0; i < rgbValues.size(); i++)
    {
        rgb_Averages.push_back({ 0,0,0 });
        rgb_Averages[0].r += rgbValues[i][0];
        rgb_Averages[0].g += rgbValues[i][1];
        rgb_Averages[0].b += rgbValues[i][2];
        std::cout << rgbValues[i][0] << " " << rgbValues[i][1] << " " << rgbValues[i][2] << "\n";
    }
    rgb_Averages[1].r = rgb_Averages[0].r / rgbValues.size();
    rgb_Averages[1].g = rgb_Averages[0].g / rgbValues.size();
    rgb_Averages[1].b = rgb_Averages[0].b / rgbValues.size();
    for (int i = 0; i < rgbValues.size(); i++)
    {

        rgb_Averages[2].r += pow((rgbValues[i][0] - rgb_Averages[1].r), 2);
        rgb_Averages[2].g += pow((rgbValues[i][1] - rgb_Averages[1].g), 2);
        rgb_Averages[2].b += pow((rgbValues[i][2] - rgb_Averages[1].b), 2);
    }
    //Calculate Standard Deviation of R,G,B Values
    int stdevR = sqrt(rgb_Averages[2].r / rgbValues.size());
    int stdevG = sqrt(rgb_Averages[2].g / rgbValues.size());
    int stdevB = sqrt(rgb_Averages[2].b / rgbValues.size());
    std::cout << "R MEAN : " << rgb_Averages[1].r << " G MEAN: " << rgb_Averages[1].g << " B MEAN: " << rgb_Averages[1].b << "\n";
    std::cout << "R STDEV: " << stdevR << "  G STDEV: " << stdevG << " B STDEV " << stdevB << "\n";
    for (int i = 0; i < rgbValues.size(); i++)
    {
        int outlier = (rgbValues[i][2] < rgb_Averages[1].b) ? (rgbValues[i][2] + stdevB) : (rgbValues[i][2] - stdevB);
        std::cout << "Deviation " << outlier << std::endl;
        if (abs(outlier - rgb_Averages[1].b) > stdevB) {
            std::cout << "Outlier: " << rgbValues[1][2];
        }
    }

}


//int findSlot(int width, int height, float UISCALE, double resScale)
//{
//    HDC hScreen = GetDC(NULL);
//    int result = 0;
//    int avgRGBBlue = 0;
//    double widthScale = (double(width) / 1920.0);
//    //double heightScale = (double(height) / 1080.0);
//    int mid_x = 660.0 * 1.0 / UISCALE * (widthScale);
//    int mid_y = 96.0 * UISCALE * (widthScale);
//    int sizeOfSlot = 90.0 * UISCALE * (widthScale);
//    int ypixel = (height)-(27.0 * widthScale * UISCALE) - sizeOfSlot;
//    for (int i = 0; i < 6; i++) {
//        int xpixel = ((width - (24.0 * UISCALE * widthScale)) / 2.0) - ((mid_y * 6.0) / 2.0) + (mid_y * i);
//        Point start = Point(xpixel, ypixel);
//        Point scanThis = Point(start.x + (30 * resScale), start.y + (20 * resScale));
//        int SCLR = GetBValue(GetPixel(hScreen, scanThis.x, scanThis.y));
//        //fprintf(fptr, "Scanning Slot %d at X: %d Y: %d, B Value is %d\n", i, scanThis.x, scanThis.y, SCLR);
//        if (SCLR > 130 && SCLR < 170)
//        {
//            //fprintf(fptr, "Slot is: %d\n", (int)i);
//            return (i + 1.0);
//        }
//    }
//    return 0;
//}
struct weaponToIndex {
    int i;
    std::string name;
}weaponToIndex;
std::vector<struct weaponToIndex> weaponsIDX = {
    {0, "AK"},
    {1,"MP5"},
    {2, "LR300"},
    {3, "M249"},
    {4, "Semi"},
    {5, "M39"},
    {6, "Thompson"},
    {7, "CustomSMG"},
    {8, "Python"},
    {9, "Semi-Skin"},
    {10, "Thompson-Skin"},
    {11, "Custom-default"},
    {12, "M92"}
};

int main()
{
    auto start = std::chrono::high_resolution_clock::now();
    Mat img = imread("./lr.png");
    Mat scene;
    //filter_RGB_VALUES(rgbValues);
    //cvtColor(img, img, COLOR_GRAY2RGB);
    int width = img.cols;
    int height = img.rows;
    float UISCALE = 1.0;
    int probablyIndex = 0;
    double resScale = ((width * height) / (1920.0 * 1080.0));
    double widthScale = (double(width) / 1920.0);
    double heightScale = (double(height) / 1080.0);
    int mid_x = 660.0 * 1.0 / UISCALE * (widthScale);
    int mid_y = 96.0 * UISCALE * (widthScale);
    int sizeOfSlot = 90.0 * UISCALE * (widthScale);
    int ypixel = (height)-(27.0 * widthScale * UISCALE) - sizeOfSlot;
    Mat rslt, mask, savedResult;
    std::vector<KeyPoint> keypoints_obj, keypoints_scene;
    Mat descriptors_box, descriptors_sence, foregroundMask, backgroundImage, foregroundIMG;
    int highestSimilarity = 0;
    int similiarityCount = 0;
    for (int i = 0; i < 6; i++) {
        auto startIter = std::chrono::high_resolution_clock::now();
        int xpixel = ((width - (24.0 * UISCALE * widthScale)) / 2.0) - ((mid_y * 6.0) / 2.0) + (mid_y * i);
        Point start = Point(xpixel, ypixel);
        Point scanThis = Point(start.x + (13 * resScale), start.y + (30 * resScale));
        Rect cropRegion = Rect2i(Rect(start.x, start.y, mid_y, mid_y));
        Mat cropped = img(cropRegion);
        std::cout << cropped.cols << " " << cropped.rows << std::endl;
        resize(cropped, cropped, Size(), 90.0 / cropped.cols, 90.0 / cropped.rows);

        std::cout << cropped.cols << " " << cropped.rows << std::endl;
        //waitKey
        resize(cropped, cropped, Size(), 3, 3);
        imshow("cropped", cropped);
        //imshow("resized", cropped);
        std::cout << cropped.cols << " " << cropped.rows << std::endl;
        //waitKey();
        //circle(img, scanThis, 2, Scalar(0, 255, 0), 1);
        //threshold(cropped, cropped, 130, 220, THRESH_BINARY);
        //cv::Mat holes = cropped.clone();
        /*imshow("threshed1", cropped);
        waitKey();*/
        savedResult = cropped;
        Mat dst, detectedEdges;
        //GaussianBlur(cropped, cropped, Size(), 3, 3);
        inRange(cropped, Scalar(130, 0, 0), Scalar(250, 150, 150), mask);
        Scalar color = mean(mask, noArray());
        dst = Scalar::all(0);

        //Mat im_in = mask;
        //rectangle(im_in, Rect2i(Rect(185, 190, 60, 60)), Scalar(255, 255, 255), -1);
        //rectangle(im_in, Rect2i(Rect(25, 15, 20, 60)), Scalar(255, 255, 255), -1);
        floodFill(mask, Point(0, 0), Scalar(255));
        //Mat im_th;
        //threshold(im_in, im_th, 220, 255, THRESH_BINARY_INV);

        //// Floodfill from point (0, 0)
        //Mat im_floodfill = im_th.clone();
        //floodFill(im_floodfill, cv::Point(0, 0), Scalar(255));

        //// Invert floodfilled image
        //Mat im_floodfill_inv;
        //bitwise_not(im_floodfill, im_floodfill_inv);
        //Mat im_out = (im_th | im_floodfill_inv);
        //imshow("imout", im_out);
        //waitKey();



        imwrite("./Mask/slotf" + std::to_string(i) + ".png", cropped);
        imwrite("./Mask/slot" + std::to_string(i) + ".png", mask);
        std::cout << " SLOT " << i + 1 << color << "\n";
        //bitwise_and(cropped, cropped, cropped, mask);
        copyTo(mask, cropped, noArray());
        //imshow("cropped", cropped);
        //waitKey();
        if (color[0] > 100) {
            std::cout << "\n" << "Testing Slot " << i + 1 << "\n";
            for (int i = 0; i < 13; i++) {
                similiarityCount = 0;
                scene = imread("./Mask/" + std::to_string(i) + ".png", IMREAD_GRAYSCALE);
                resize(scene, scene, Size(), 270.0 / scene.cols, 270.0 / scene.rows);
                Ptr<FeatureDetector> detector;
                detector = FastFeatureDetector::create();
                detector->detect(scene, keypoints_scene);
                detector->detect(cropped, keypoints_obj);

                //SIFT FEATURE EXTRACTION
                Ptr<DescriptorExtractor> extractor = SIFT::create();
                extractor->compute(scene, keypoints_scene, descriptors_sence);
                extractor->compute(cropped, keypoints_obj, descriptors_box);


                //BRUTE FORCE FOR MATCHING
                double RATIO = 0.7;
                double thresholdDistance = RATIO * sqrt(double(scene.size().height * scene.size().height + scene.size().width * scene.size().width));
                Ptr<BFMatcher> matcher = BFMatcher::create();
                std::vector<std::vector<DMatch>> matches;
                matcher->knnMatch(descriptors_sence, descriptors_box, matches, 50);
                std::vector<DMatch> good_matches;
                for (size_t i = 0; i < matches.size(); ++i)
                {
                    for (int j = 0; j < matches[i].size(); j++)
                    {
                        Point2f from = keypoints_obj[matches[i][j].queryIdx].pt;
                        Point2f to = keypoints_scene[matches[i][j].trainIdx].pt;

                        //calculate local distance for each possible match
                        double dist = sqrt((from.x - to.x) * (from.x - to.x) + (from.y - to.y) * (from.y - to.y));
                        if (matches[i][0].distance < RATIO * matches[i][1].distance)
                        {
                            good_matches.push_back(matches[i][j]);
                            similiarityCount++;
                            j = matches[i].size();
                        }
                    }
                }
                //resultVector[i][1] = std::to_string(std::atoi(resultVector[i][1].c_str()) + similiarityCount
                //std::cout << "Similiarity: " << similiarityCount << "\n";
                if (similiarityCount > highestSimilarity) {
                    Mat img_matches;
                    drawMatches(scene, keypoints_scene, cropped, keypoints_obj, good_matches, img_matches, Scalar::all(-1),
                        Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::DEFAULT);
                    highestSimilarity = similiarityCount;
                    probablyIndex = i;
                    copyTo(img_matches, savedResult, noArray());
                    std::cout << "Similiarity: " << similiarityCount << "\n";
                    imshow("Good Matches", img_matches);
                    waitKey();
                }
                //waitKey(0);
                //waitKey();
            }
            printf("Took %dms\n", (int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startIter).count());
            //waitKey();
            //imshow("./rslt.png", img);
            rectangle(img, Rect2i(Rect(start.x, start.y, mid_y - 5, mid_y - 5)), Scalar(0, 0, 255), 3);
        }
        else {
            rectangle(img, Rect2i(Rect(start.x, start.y, mid_y - 5, mid_y - 5)), Scalar(255, 0, 0), 3);
        }
        //line(img, Point(start.x, start.y), Point(scanThis.x + 10, scanThis.y), Scalar(0, 0, 255), 3);
        //circle(img, Point(start.x + 30, start.y + 40), 10, Scalar(255, 0, 0), 2);
    }
    imshow("img", savedResult);
    resize(img, img, Size(), 1920.0 / img.cols, 1080.0 / img.rows);
    imwrite("Screen.png", img);
    imshow("imgg", img);
    std::cout << weaponsIDX[probablyIndex].name << "Highest Similarity " << highestSimilarity << "\n";
    printf("Took %dms\n", (int)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count());
    waitKey();
}


