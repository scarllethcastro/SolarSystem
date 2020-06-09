#pragma once
const float km = 1.0e-7;
const float kminv = 1.0e7;
const float m = 1.0e-3 * km;
const float minv = kminv/1000;
const float s = 1/(3600*24*30); //time in months
const float sinv = 3600*24*30;
const float h = 3600*s;
const float hinv = sinv/3600;
const float kg = 1.0e-27;
const float kginv = 1.0e+27;


// Gravitational constant
const float G = 6.674 * (1.0e-11) * (m*m*m) * kginv *sinv *sinv;//0.00000017299008f;

// SUN
float sun_radius = 696340 * km;//0.069634;
float sun_mass = 1.989 * 1e+30 * kg;

// EARTH
float e_radius = 6400 *km;
float e_mass = 5.972 * 1e+24 * kg;//0.005972;
float e_inclination = -23.4f * 3.14f/180.0f;
float e_orbitradius = 14.96;
vcl::vec3 e_p = {147.1*(1.0e+6)* km, 0, 0,};
vcl::vec3 e_v = {0, 110700 *km*hinv,0};


