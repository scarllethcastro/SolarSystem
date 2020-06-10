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
const float G = 6.674 * (1.0e-11) * (m*m*m) * kginv *sinv *sinv;

// SUN
float sun_radius = 696340 * km;//0.069634;
float sun_mass = 1.989 * 1e+30 * kg;

const float vel_aux = 2*G*sun_mass; //for velocity calculation

//MERCURY
float m_radius = 2439.7 *km;
float m_mass = 3.303 * 1.0e+23 * kg;
float m_inclination = 0.0f;
float m_orbitradius = 57909176 * km;
float m_rp = 46001272* km;
vcl::vec3 m_p = {m_rp ,0,0};
float m_vp = sqrt(vel_aux *(1/m_rp - 1/(2*m_orbitradius)));
vcl::vec3 m_v = {0,m_vp, 0};
vcl::vec3 m_force = G * sun_mass * m_mass/(norm(m_p)*norm(m_p)) * -1.0f *normalize(m_p);

//VENUS
float v_radius = 6050.6 *km;
float v_mass = 4.8685 * 1e+24 * kg;
float v_inclination = 177.36 * 3.14/180;
float v_orbitradius = 108208930 * km;
float v_rp = 107476000 * km;
float v_orbitinclination = 3.394 * 3.14/180;
vcl::vec3 v_p = {v_rp, 0, v_orbitinclination*v_rp};
float v_vp = sqrt(vel_aux * (1/v_rp - 1/(2*v_orbitradius)));
vcl::vec3 v_v = {0, v_vp, 0};
vcl::vec3 v_force = G * sun_mass * v_mass/(norm(v_p)*norm(v_p)) * -1.0f *normalize(v_p);

//EARTH
float e_radius = 6400 *km;
float e_mass = 5.972 * 1e+24 * kg;//0.005972;
float e_inclination = -23.4f * 3.14f/180.0f;
float e_orbitradius = 14.96;
vcl::vec3 e_p = {147.1*(1.0e+6)* km, 0, 0};
vcl::vec3 e_v = {0, 110700 *km*hinv,0};
vcl::vec3 e_force = G * sun_mass * e_mass/(norm(e_p)*norm(e_p)) * -1.0f *normalize(e_p);


//MARS
float ma_radius = 3397.2 * km;
float ma_mass = 6.4174 * 1e+23 * kg;
float ma_inclination = 5.65*3.14/180;
float ma_orbitradius = 227939100 *km;
float ma_orbitinclination = 1.850*3.14/180;
float ma_rp = 206669000 * km;
vcl::vec3 ma_p = {ma_rp, 0, ma_orbitinclination * ma_rp};
float ma_vp = sqrt(vel_aux * (1/ma_rp - 1/(2*ma_orbitradius)));
vcl::vec3 ma_v = {0, ma_vp, 0};
vcl::vec3 ma_force = G * sun_mass * ma_mass/(norm(ma_p)*norm(ma_p)) * -1.0f *normalize(ma_p);

//JUPITER
float j_radius = 71492 *km;
float j_mass = 1.9*1e+27 * kg;
float j_inclination = 3.13 * 3.14/180;
float j_orbitinclination = 1.305*3.14/180;
float j_orbitradius = 778547200 * km;
float j_rp = 740573600 * km;
vcl::vec3 j_p = {j_rp, 0, j_orbitinclination * j_rp};
float j_vp = sqrt(vel_aux * (1/j_rp - 1/(2*j_orbitradius)));
vcl::vec3 j_v ={0 , j_vp, 0};
vcl::vec3 j_force = G * sun_mass * j_mass/(norm(j_p)*norm(j_p)) * -1.0f *normalize(j_p);

//SATURN
float s_radius = 60268 * km;
float s_mass = 5.6846 * 1.0e+26;
float s_inclination = 5.51 *3.14/180;
float s_orbitinclination = 2.49 * 3.14/180;
float s_orbitradius = 1433449370 *km;
float s_rp = 1353572956 * km;
vcl::vec3 s_p = {s_rp, 0, s_orbitinclination*s_rp};
float s_vp = sqrt(vel_aux * (1/s_rp - 1/(2*s_orbitradius)));
vcl::vec3 s_v = {0,s_vp, 0};
vcl::vec3 s_force = G * sun_mass * s_mass/(norm(s_p)*norm(s_p)) * -1.0f *normalize(s_p);

//SATURN RING
float sr_int_radius = 67000 * km;
float sr_ext_radius = 140210 * km;

//URANUS
float u_radius = 25.559 * km;
float u_mass = 8.686 * 1.0e+25;
float u_inclination = 97.77 *3.14/180;
float u_orbitinclination = 0.774 * 3.14/180;
float u_orbitradius = 2876679082 *km;
float u_rp = 2748938461 * km;
vcl::vec3 u_p = {u_rp, 0, u_orbitinclination*u_rp};
float u_vp = sqrt(vel_aux * (1/u_rp - 1/(2*u_orbitradius)));
vcl::vec3 u_v = {0,u_vp, 0};
vcl::vec3 u_force = G * sun_mass *u_mass/(norm(u_p)*norm(u_p)) * -1.0f *normalize(u_p);

//NEPTUNE
float n_radius = 24.746 * km;
float n_mass = 1.024 * 1.0e+26;
float n_inclination = 28.31 * 3.14/180;
float n_orbitinclination = 1.774 * 3.14/180;
float n_orbitradius = 4503443661 *km;
float n_rp = 4452940833 * km;
vcl::vec3 n_p = {n_rp, 0, n_orbitinclination*n_rp};
float n_vp = sqrt(vel_aux * (1/n_rp - 1/(2*n_orbitradius)));
vcl::vec3 n_v = {0,n_vp, 0};
vcl::vec3 n_force = G * sun_mass * n_mass/(norm(n_p)*norm(n_p)) * -1.0f *normalize(n_p);
