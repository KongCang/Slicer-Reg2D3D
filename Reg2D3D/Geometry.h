#ifndef GEOMETRY_H
#define GEOMETRY_H


class Geometry
{
public:
    Geometry(float focal_width, int intensityDivider) {this->focal_width=focal_width;this->intensityDivider=intensityDivider;}
    ~Geometry() {}
    float GetFocalWidth(){return focal_width;}
    void SetFocalWidth(float focal_width) {this->focal_width=focal_width;}
    int GetIntensityDivider(){return intensityDivider;}
    void SetIntensityDivider(int intensityDivider){this->intensityDivider=intensityDivider;}

private:
    float focal_width;
    int intensityDivider;
};




#endif //GEOMETRY_H
