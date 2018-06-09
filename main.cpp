#include <iostream>
#include <fstream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <jsoncpp/json/json.h>

using namespace std;
using namespace cv;

#define WIN_NAME "mespts"
#define SCALE 2

static vector<Point2i> points;

void
CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    switch (event)
    {
        case EVENT_LBUTTONDOWN:
            points.push_back(Point2i(x/SCALE, y/SCALE));
            break;
        case EVENT_MOUSEWHEEL:
            cout <<  getMouseWheelDelta(flags) << " weel A x" << x << " y " << y << endl;
            break;
    }
}

void
wait_esc_key()
{
    while (waitKey(0) != 27)
    {
        /* nop */
    }
}

void
test_json()
{
    Json::Value val;
    Json::Reader reader;

    ifstream jsonfile("pts.json");
    if (!reader.parse(jsonfile, val))
    {
        cout << "Error: " << reader.getFormattedErrorMessages();
        return;
    }

    auto json_pts = val["points"];
    for (int i = 0; i < json_pts.size(); i += 1)
    {
        cout << json_pts[i]["x"] << endl;
    }
}

bool
file_names(int argc, char* argv[], string & img_file, string & json_file)
{
    if (argc != 2)
    {
        cout << "usage: " << argv[0] << " <png_file>\n";
        return false;
    }

    /* image file from first cli argument */
    img_file = argv[1];

    /*
     * json file is derived from image file name,
     * where we replace file name extension with '.json'
     * e.g. img.png -> img.json
     */
    auto last_dot = img_file.find_last_of(".");
    if (last_dot == string::npos)
    {
        cout << "dot not found in file path" << img_file;
        return false;
    }
    json_file = img_file.substr(0, last_dot) + ".json";

    return true;
}

void
write_json(string fname, Json::Value & json_pts)
{
    ofstream jsonfile(fname);

    Json::Value json;
    json["points"] = json_pts;

    jsonfile << json;
    jsonfile.close();
}

int
main(int argc, char* argv[])
{
    string img_file;
    string json_file;

    if (!file_names(argc, argv, img_file, json_file))
    {
        return -1;
    }

    Mat orig = imread(img_file);
    Mat scaled;

    resize(orig, scaled, Size(), SCALE, SCALE);

    namedWindow(WIN_NAME, 1);
    setMouseCallback(WIN_NAME, CallBackFunc, NULL);
    imshow(WIN_NAME, scaled);

    wait_esc_key();

    auto color = Scalar(0, 0, 0);
    Json::Value json_pts;
    for (int i = 0; i < points.size(); i += 1)
    {
        auto pt = points[i];
        json_pts[i]["x"] = pt.x;
        json_pts[i]["y"] = pt.y;
        circle(orig, pt, 3, color, 3);
    }

    write_json(json_file, json_pts);

    imshow(WIN_NAME, orig);
    wait_esc_key();
}
