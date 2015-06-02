#include "TemperatureProfile2D.h"
#include "Tools.h"

using namespace std;

TemperatureProfile2D::TemperatureProfile2D(ProfileSpecies &my_prof_params) : TemperatureProfile(my_prof_params) {
    
    // -------------------------
    // Constant Temperature profile
    // -------------------------
    // vacuum_length[0] : length of the vacuum region before the plasma (default is 0)
    // dens_length_x[0]   : length of the density (default is sim_length-vacuum_length[0])
    if (prof_params.profile=="constant") {
        // nothing done here, by default: vacuum_length[0] = 0, dens_length_x[0] = 0
    }
    // ---------------------------------------------------------------------------------
    // Charles magnetic field profile for Liang simulations
    // vacuum_length[0]  : not used here
    // double_params[0]   = background density
    // double_params[1]   = Total plasma pressure at infinity P0 = n0*(Te + Ti +...)
    // double_params[2]   = background magnetic field
    // double_params[3]   = Maximum magnetic field
    // length_params_x[0] = position of the maximum magnetic field
    // length_params_x[1] = Length of the magnetic gradient
    // ---------------------------------------------------------------------------------
    else if (prof_params.profile == "magexpansion") {
        //if (prof_params.int_params.size()<2)
        //    ERROR("two int_params must be defined for Charles velocity profile" );
        if (prof_params.double_params.size()<5)
            ERROR("five double_params must be defined for magexpansion Temperature profile" );
        if (prof_params.length_params_y.size()<2)
            ERROR("two length_params_y must be defined for magexpansion Temperature profile" );
    }
    
    // ---------------------------------------------------------------------------------
    // Blob magnetic field profile for Liang simulations
    // double_params[0]   = background density
    // double_params[1]   = relative density variation
    // double_params[2]   = Total plasma pressure
    // double_params[3]   = background magnetic field
    // double_params[4]   = Maximum magnetic field
    // length_params_x[0] = x position of the maximum of the B-field
    // length_params_x[1] = Length of the magnetic gradient
    // length_params_y[2] = y position of the maximum of the B-field
    // ---------------------------------------------------------------------------------
    else if (prof_params.profile == "blob") {
        if (prof_params.double_params.size()<5)
            ERROR("five double_params_x must be defined for magexpansion Temperature profile" );
        if (prof_params.length_params_x.size()<2)
            ERROR("two length_params_y must be defined for magexpansion Temperature profile" );
        if (prof_params.length_params_y.size()<1)
            ERROR("two length_params_y must be defined for magexpansion Temperature profile" );
    }
    
}

double TemperatureProfile2D::operator() (vector<double> x_cell) {
    
    double fx, fy;
    // ------------------------
    // Constant density profile
    // ------------------------
    // vacuum_length[0] : length of the vacuum region before the plasma (default is 0)
    // dens_length_x[0]   : length of the density (default is sim_length-vacuum_length[0])
    if (prof_params.profile=="constant") {
        // x-direction
        if (   (x_cell[0]>prof_params.vacuum_length[0])
            && (x_cell[0]<prof_params.vacuum_length[0]+prof_params.length_params_x[0]) ) {
            fx = 1.0;
        } else {
            fx = 0.0;
        }
        
        // y-direction
        if (   (x_cell[1]>prof_params.vacuum_length[1])
            && (x_cell[1]<prof_params.vacuum_length[1]+prof_params.length_params_y[0]) ) {
            fy = 1.0;
        } else {
            fy = 0.0;
        }
        
        // x-y direction
        return fx*fy;
    }
    
    
    // ------------------------
    // Charles temperature profile
    // ------------------------
    // vacuum_length[0]  : not used here
    // double_params[0]   = background density
    // double_params[1]   = Total plasma pressure at infinity P0 = n0*(Te + Ti +...)
    // double_params[2]   = background magnetic field
    // double_params[3]   = Maximum magnetic field
    // double_params[4]   = Temperature gradient paramter -> 0 for the true equilibrium
    // length_params_x[0] = position of the maximum magnetic field
    // length_params_x[1] = Length of the magnetic gradient
    // ---------------------------------------------------------------------------------
    else if (prof_params.profile=="magexpansion") {
        double n0    = prof_params.double_params[0];
        double P0    = prof_params.double_params[1];
        double B0    = prof_params.double_params[2];
        double Bmax  = prof_params.double_params[3];
        double alpha = prof_params.double_params[4];
        //if (prof_params.double_params.size>4) { 
        //	alpha = prof_params.double_params[4];
        //	}
        //else { alpha = 0.;}
        double x0    = prof_params.length_params_y[0];
        double L     = prof_params.length_params_y[1];
        double x     = x_cell[1]-x0;
        double tiny  = 1e-10;
        if (Bmax == 0.) { //-> maximum value of Bmax
            double Bm = sqrt(pow(B0,2) + 2*P0)-B0;
            double B  = B0 + Bm/pow(cosh(x/L),2);
            double A  = B0*x + Bm*L*tanh(x/L);
            double Amin  = B0*L*tiny + Bm*L*tanh(tiny);
            double DP = P0 + pow(B0,2)/2 - pow(B,2)/2;
            double DP_min =P0 + pow(B0,2)/2-pow(B0 + Bm/pow(cosh(tiny),2),2 )/2;
            
            double Temp     = DP/n0*exp( 2*A*Bm/L*tanh(x/L) /(DP*pow(cosh(x/L),2)) )*(1+tanh(alpha*x/L));
            double Tempmin  = DP_min/n0*exp( 2*Amin*Bm/L*tanh(tiny) /(DP_min*pow(cosh(tiny),2)) );
        	if (Temp<0.) ERROR("Temperature smaller than 0 imposed in profile magexpansion");
            return max(Temp,Tempmin);
		}
        else {	
            double Bm = Bmax;
            double B  = B0 + Bm/pow(cosh(x/L),2);
            double A  = B0*x + Bm*L*tanh(x/L);
            double Amin  = B0*L*tiny + Bm*L*tanh(tiny);
            double DP = P0 + pow(B0,2)/2 - pow(B,2)/2;
            double DP_min =P0 + pow(B0,2)/2-pow(B0 + Bm/pow(cosh(tiny),2),2 )/2;
            
            double Temp     = DP/n0*exp( 2*A*Bm/L*tanh(x/L) /(DP*pow(cosh(x/L),2)) )*(1+tanh(alpha*x/L));
            double Tempmin  = DP_min/n0*exp( 2*Amin*Bm/L*tanh(tiny) /(DP_min*pow(cosh(tiny),2)) );
        	if (Temp<0.) ERROR("Temperature profile smaller than 0 imposed in profile magexpansion");
            return  max(Temp,Tempmin);
		}
	}
	
	
    // ---------------------------------------------------------------------------------
    // Blob magnetic field profile for Liang simulations
    // double_params[0]   = background density
    // double_params[1]   = relative density variation
    // double_params[2]   = Total electron pressure n0*Te0
    // double_params[3]   = background magnetic field
    // double_params[4]   = Maximum magnetic field
    // length_params_x[0] = x position of the maximum of the B-field
    // length_params_x[1] = Length of the magnetic gradient
    // length_params_y[2] = y position of the maximum of the B-field
    // ---------------------------------------------------------------------------------
    else if (prof_params.profile=="blob") {
        double n0    = prof_params.double_params[0];
        double dn    = prof_params.double_params[1];
        double P0    = prof_params.double_params[2];
        double B0    = prof_params.double_params[3];
        double Bmax  = prof_params.double_params[4];
        double x0    = prof_params.length_params_x[0];
        double y0    = prof_params.length_params_y[0];
        double L     = prof_params.length_params_x[1];
        double  r    = sqrt(pow(x_cell[0]-x0,2) + pow(x_cell[1]-y0,2));
        double ne    = n0*(1 - dn/(pow(cosh(r/L),2)));
        double tiny  = 1e-10;
        double Er;
        if (r/L>tiny) {
            Er    = n0*dn*(L*tanh(r/L) - pow(L,2)/(r+1e-10)*log(cosh(r/L)) );
        }else{
            Er    = 0.5*n0*r;
        }
        
        if (Bmax == 0.) { //-> maximum value of Bmax
            double Bm = sqrt(pow(B0,2) + 2*P0)-B0;
            double B  = B0 + Bm/pow(cosh(r/L),2);
            double Temp = P0/ne + 0.5*(pow(B0,2) - pow(B,2))/ne  - 0.5*pow(Er,2)/ne;
         	//if (Temp<0.) ERROR("Temperature smaller than 0 imposed in profile magexpansion");
         	if (Temp<0.) MESSAGE("Temperature smaller than 0 imposed in profile magexpansion");
            return max(Temp,P0/n0/1000);
		}
        else {	
            double Bm = Bmax;
            double B  = B0 + Bm/pow(cosh(r/L),2);
            double Temp = P0/ne + 0.5*(pow(B0,2) - pow(B,2))/ne  - 0.5*pow(Er,2)/ne;
         	if (Temp<0.) ERROR("Temperature smaller than 0 imposed in profile magexpansion");
            return max(Temp,P0/n0/1000);
		}
	}
    else if (prof_params.profile=="python") {
        return PyTools::runPyFunction(prof_params.py_profile, x_cell[0], x_cell[1]);
    }
    
    return 1;
};