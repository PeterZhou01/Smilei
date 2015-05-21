#ifndef LaserProfile_H
#define LaserProfile_H

#include <cmath>

#include "PicParams.h"
#include "LaserParams.h"
#include "pyHelper.h"


class LaserProfile {
public:

    LaserProfile( PicParams &params, LaserParams &laser_params, unsigned int );

    LaserStructure laser_struct;

    double pi_ov_2;
    double PI2;
    
    double omega0_;
    double tchirp_;
    
    double a0_delta_y_;
    double a0_delta_z_;

    std::string         type_of_time_profile;
    std::string         type_of_y_profile;
    std::vector<int>    int_params;
    std::vector<double> double_params;

    std::string         type_of_transv_profile;
    std::vector<int>    int_params_transv;
    std::vector<double> double_params_transv;
    
    double time_profile(double);
    double transverse_profile2D(double, double);

    void disabled();

};

#endif

