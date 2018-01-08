//
//  main.cpp
//  bike_light
//
//  Created by Gregory C Lewin on 1/5/18.
//  Copyright © 2018 Gregory C Lewin. All rights reserved.
//

#include <iostream>
#include <math.h>

#define DT 0.0001

//V1    0.118
//V3    0.039
//Ri    4.2
//Li    0.18

#define V_rect 0.6

class BikeLight
{
public:
    double I_s = 0, I_r = 0, I_c = 0, I_led = 0; //I_r and I_s should be equal upon convergence
    double V_s = 0, V_lr = 0, V_1 = 0, V_2 = 0; //V2 is always zero, by definition
    double V_lj = 0, V_rj = 0, V_c = 0;
    
    double L_i = 0.18; //Henry
    double C = 4700e-6; //Farad
    double R_i = 4.2;
    double R_led = 10; //Ohmic for the moment...
    
    double a1 = 0.118;
    double a3 = 0;//-0.039; //negative to get it in phase
    double omega = 300;
    
    double Tk = 0;
    
    void Print(void)
    {
        std::cout << V_s << '\t' << V_1 << '\t' << V_lr << '\t' << V_c << '\t' << I_s << '\t' << I_c << '\t' << I_led << '\n';
    }
    
    int Step(double Vs, double dt)
    {
        V_s = Vs;
        //store previous step's info
        I_c = fabs(I_s) - I_led;
        double dVc = dt * I_c / C;

        double Is = I_s;
        //double dIs = dt * (V_s - V_lr) / L_i;
        
        V_c += dVc;
        I_led = CalcLEDCurrent(V_c, 3);
        
        unsigned long i = 10000ul;
        double alpha = 0.1;
        while(--i)
        {
            //conditional from the right
            if(V_1 >= (V_c + V_rect)) //forward Is, but smooth out rapid changes
            {
                V_1 = V_c + V_rect;
                I_r = I_c + I_led; //(V_lr - V_1) / R_i;
            }
            
            else if(V_1 <= -(V_c + V_rect)) // backward Is, but smooth out rapid changes
            {
                V_1 = -(V_c + V_rect);
                I_r = I_c + I_led; //(V_lr - V_1) / R_i;
            }
            
            else //zero Is, but try to smooth it out
            {
                V_1 = V_1; //just for reference
                I_r = 0;
            }
            
            //so we have I_r from the right; now find Is from the left
            Is = I_s + dt * (V_s - V_1 - I_s * R_i) / L_i;
            
            //now adjust V_rl to make them all equal
            V_1 += (Is - I_r) * alpha; //the R_i is non-physical; just trying to make it more general
            V_lr = (V_1 + Is * R_i);
        }
        
        I_s = Is;
        
        return 1;
    }

    double CalcVs(double t)
    {
        double Vs = a1 * omega * sin (omega * t) + a3 * omega * sin (3 * omega * t);
        
        return Vs;
    }
    
    double CalcLEDCurrent(double V, int nLEDs)
    {
        double v = V / nLEDs;
        if(v > 2.55) return (v - 2.55) * 1.750;
        else return 0;
    }
};



int main(int argc, const char * argv[])
{
    // insert code here...
//    std::cout << "Hello, World!\n";
    
    BikeLight light;
    
    for(int i = 0; i < 10000; i++)
    {
        light.Step(light.CalcVs(i * DT), DT);
        
        std::cout << i << '\t';
        light.Print();
    }
    
    return 0;
}
