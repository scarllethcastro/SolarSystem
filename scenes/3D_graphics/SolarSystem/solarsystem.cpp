

#include "solarsystem.hpp"

#include "data.hpp"

#include <cmath>

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;


mesh create_universe(float dimension);
star& create_star(float radius, float mass, vcl::vec3 p, vcl::vec3 v);
planet& create_planet(float radius, float mass, vcl::vec3 p, vcl::vec3 v,  float inclination, vcl::vec3 force, float orbit_radius, float vel_rot);
mesh create_ring(float r_int, float r_ext);


/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_model::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure&)
{
    // Setup initial camera mode and position
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 25.0f;
    scene.camera.apply_rotation(0,0,0,1.2f);

    // Universe creation
    setup_universe();

    // Sun creation
    setup_sun();

    // Planets creation
    setup_mercury();
    setup_venus();
    setup_earth();
    setup_mars();
    setup_jupiter();
    setup_saturn();
    setup_uranus();
    setup_neptune();

    // Moon creation
    setup_moon();
}


/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure&)
{
    timer.update();
    set_gui(timer);

    // Simulation time step (dt)
    float dt = timer.scale*0.001f;

    glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe


    /// *** Data update *** ///

    // Planets
    update_position_planets(dt);

    // Moon
    update_position_moon(dt);

    // Saturn ring
    update_position_saturn_ring();

    /// ******************* ///


    /// *** Follow the star selected on Gui *** ///

    camera_position_at_each_star(scene);

    /// *************************************** ///


    /// *** Draw the elements *** ///

    // Universe
    draw_universe(shaders, scene);
    // Sun
    draw_sun(shaders, scene);
    // Planets
    draw_planets(shaders, scene);
    // Moon
    draw_moon(shaders, scene);
    // Saturn ring
    draw_saturn_ring(shaders, scene);
    // Sun ring
    draw_sun_ring(shaders, scene);

    /// ************************* ///


    /// *** Wireframe *** ///

    if( gui_scene.wireframe ){ // wireframe if asked from the GUI
        glPolygonOffset( 1.0, 1.0 );
        // Universe
        draw(universe, scene.camera, shaders["wireframe"]);
        // Sun
        draw(sun.drawable,scene.camera, shaders["wireframe"]);
        // Planets
        for(planet& it : planets){
            draw(it.drawable, scene.camera, shaders["wireframe"]);
        }
        // Moon
        draw(moon.drawable, scene.camera, shaders["wireframe"]);
        // Saturn ring
        draw(saturn_ring.drawable, scene.camera, shaders["wireframe"]);
        // Sun ring
        draw(sun_ring, scene.camera, shaders["wireframe"]);
    }

    /// ***************** ///
}


// ************************** //
// CREATION FUNCTIONS
// ************************** //

star& create_star(float radius, float mass, vec3 p, vec3 v){
    static star new_star;
    new_star.mass = mass;
    new_star.radius = radius;
    new_star.p = p;
    new_star.v = v;

    return new_star;
}

planet& create_planet(float radius, float mass, vec3 p, vec3 v,  float inclination, vec3 force, float orbit_radius, float vel_rot){
    static planet new_planet;
    new_planet.mass = mass;
    new_planet.radius = radius;
    new_planet.p = p;
    new_planet.v = v;
    new_planet.vel_rot = vel_rot;

    new_planet.inclination = inclination;
    new_planet.force = force;
    new_planet.orbit_radius = orbit_radius;
    //new_planet.drawable.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,1,0}, inclination);

    return new_planet;

}

mesh create_universe(float dimension){
    mesh sky;
    // Geometry
    float l = dimension/2.0f;
    mesh base;
    mesh face1;
    mesh face2;
    mesh face3;
    mesh face4;
    mesh top;

    base.position = {{-l,-l,-l}, {l,-l,-l}, {l,l,-l}, {-l,l,-l}};
    base.texture_uv = {{1/4.0f, 2/3.0f}, {2/4.0f, 2/3.0f}, {2/4.0f, 1.0f}, {1/4.0f, 1.0f}};
    base.connectivity = {{0,1,2}, {0,2,3}};

    face1.position = {{-l,-l,-l}, {l,-l,-l}, {-l,-l,l}, {l,-l,l}};
    face1.texture_uv = {{1/4.0f, 2/3.0f}, {2/4.0f, 2/3.0f}, {1/4.0f, 1/3.0f}, {2/4.0f, 1/3.0f}};
    face1.connectivity = {{0,1,2}, {1,2,3}};

    face2.position = {{l,-l,-l}, {l,l,-l}, {l,-l,l}, {l,l,l}};
    face2.texture_uv = {{2/4.0f, 2/3.0f}, {3/4.0f, 2/3.0f}, {2/4.0f, 1/3.0f}, {3/4.0f, 1/3.0f}};
    face2.connectivity = {{0,1,2}, {1,2,3}};

    face3.position = {{l,l,-l}, {-l,l,-l}, {l,l,l}, {-l,l,l}};
    face3.texture_uv = {{3/4.0f, 2/3.0f}, {1.0f, 2/3.0f}, {3/4.0f, 1/3.0f}, {1.0f, 1/3.0f}};
    face3.connectivity = {{0,1,2}, {1,2,3}};

    face4.position = {{-l,l,-l}, {-l,-l,-l}, {-l,-l,l}, {-l,l,l}};
    face4.texture_uv = {{0, 2/3.0f}, {1/4.0f, 2/3.0f}, {1/4.0f, 1/3.0f}, {0, 1/3.0f}};
    face4.connectivity = {{0,1,2}, {0,2,3}};

    top.position = {{-l,-l,l}, {l,-l,l}, {l,l,l}, {-l,l,l}};
    top.texture_uv = {{1/4.0f, 1/3.0f}, {2/4.0f, 1/3.0f}, {2/4.0f, 0}, {1/4.0f, 0}};
    top.connectivity = {{0,1,2}, {0,2,3}};

    sky.push_back(base);
    sky.push_back(face1);
    sky.push_back(face2);
    sky.push_back(face3);
    sky.push_back(face4);
    sky.push_back(top);
    return sky;
}

mesh create_ring(float r_int, float r_ext)
{
    mesh ring;

    // Number of retangles
    size_t N = 100;

    // Minimum angle
    float theta = 2*3.14f/float(N);

    for(uint i = 0; i < N; i++){
        // Geometry
        vec3 point0 = {r_int*std::cos(i*theta),r_int*std::sin(i*theta),0};
        vec3 point1 = {r_ext*std::cos(i*theta),r_ext*std::sin(i*theta),0};
        vec3 point2 = {r_int*std::cos((i+1)*theta),r_int*std::sin((i+1)*theta),0};
        vec3 point3 = {r_ext*std::cos((i+1)*theta),r_ext*std::sin((i+1)*theta),0};
        ring.position.push_back(point0);
        ring.position.push_back(point1);
        ring.position.push_back(point2);
        ring.position.push_back(point3);

        // Connectivity
        ring.connectivity.push_back({i*4,i*4+1,i*4+2});
        ring.connectivity.push_back({i*4+1,i*4+2,i*4+3});

        // Texture
        ring.texture_uv.push_back(vec2(0,1));
        ring.texture_uv.push_back(vec2(1,1));
        ring.texture_uv.push_back(vec2(0,0));
        ring.texture_uv.push_back(vec2(1,0));
    }

    return ring;
}


// ************************** //
// SETUP FUNCTIONS
// ************************** //

void scene_model::setup_universe()
{
    universe = create_universe(1100.0f);
    universe.uniform.shading = {1,0,0};
    universe.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/universe/8k_stars_milky.png"));
}

void scene_model::setup_sun()
{
    // Sun
    sun = create_star(sun_radius, sun_mass, {0,0,0} , {0,0,0});
    sun.drawable = mesh_primitive_sphere(sun_radius*200, {0,0,0}, 40, 80);
    sun.drawable.uniform.shading = {1,0,0};
    sun.drawable.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/sun/8k_sun.png"));
    // Sun ring
    mesh sunring;
    float size = 325.0f;
    sunring.position     = {{-sun_radius*size,-sun_radius*size,0}, {sun_radius*size,-sun_radius*size,0}, {sun_radius*size,sun_radius*size,0}, {-sun_radius*size,sun_radius*size,0}};
    sunring.texture_uv   = {{0,1}, {1,1}, {1,0}, {0,0}};
    sunring.connectivity = {{0,1,2}, {0,2,3}};
    sun_ring = sunring;
    sun_ring.uniform.shading = {1,0,0};
    // Load a texture (with transparent background)
    sun_ring.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/sun/sun_ring.png"), GL_REPEAT, GL_REPEAT );
}

void scene_model::setup_mercury()
{
    planet mercury;
    mercury = create_planet(m_radius, m_mass, m_p, m_v, m_inclination, m_force, m_orbitradius, m_vel_rot);
    mercury.drawable = mesh_primitive_sphere(m_radius*1000, {0,0,0}, 40, 80);
    mercury.drawable.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,1,0}, m_inclination);
    mercury.drawable.uniform.shading.specular = 0.0f;
    mercury.drawable.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/mercury/8k_mercury.png"));
    planets.push_back(mercury);
}

void scene_model::setup_venus()
{
    planet venus;
    venus = create_planet(v_radius, v_mass,v_p, v_v, v_inclination, v_force, v_orbitradius, v_vel_rot);
    venus.drawable = mesh_primitive_sphere(v_radius*1000, {0,0,0}, 40, 80);
    venus.drawable.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,1,0}, v_inclination);
    venus.drawable.uniform.shading.specular = 0.0f;
    venus.drawable.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/venus/4k_venus.png"));
    planets.push_back(venus);
}

void scene_model::setup_earth()
{
    planet earth;
    earth = create_planet(e_radius, e_mass, e_p, e_v, e_inclination, e_force, e_orbitradius, e_vel_rot);
    earth.drawable = mesh_primitive_sphere(e_radius*1000, {0,0,0}, 40, 80);
    earth.drawable.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,1,0}, e_inclination);
    earth.drawable.uniform.shading.specular = 0.0f;
    earth.drawable.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/earth/8k_earth.png"));
    planets.push_back(earth);
}

void scene_model::setup_mars()
{
    planet mars;
    mars = create_planet(ma_radius, ma_mass, ma_p, ma_v, ma_inclination, ma_force, ma_orbitradius, ma_vel_rot);
    mars.drawable = mesh_primitive_sphere(ma_radius*1000, {0,0,0}, 40, 80);
    mars.drawable.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,1,0}, ma_inclination);
    mars.drawable.uniform.shading.specular = 0.0f;
    mars.drawable.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/mars/8k_mars.png"));
    planets.push_back(mars);
}

void scene_model::setup_jupiter()
{
    planet jupiter;
    jupiter = create_planet(j_radius, j_mass, j_p, j_v, j_inclination, j_force, j_orbitradius, j_vel_rot);
    jupiter.drawable = mesh_primitive_sphere(j_radius*1000, {0,0,0}, 40, 80);
    jupiter.drawable.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,1,0}, j_inclination);
    jupiter.drawable.uniform.shading.specular = 0.0f;
    jupiter.drawable.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/jupiter/8k_jupiter.png"));
    planets.push_back(jupiter);
}

void scene_model::setup_saturn()
{
    planet saturn;
    saturn = create_planet(s_radius, s_mass, s_p, s_v, s_inclination, s_force, s_orbitradius, s_vel_rot);
    saturn.drawable = mesh_primitive_sphere(s_radius*1000, {0,0,0}, 40, 80);
    saturn.drawable.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,1,0}, s_inclination);
    saturn.drawable.uniform.shading.specular = 0.0f;
    saturn.drawable.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/saturn/8k_saturn.png"));
    planets.push_back(saturn);
    // Saturn ring
    saturn_ring = create_planet(s_radius, s_mass, s_p, s_v, s_inclination, s_force, s_orbitradius, s_vel_rot);
    saturn_ring.drawable = create_ring(sr_int_radius*1000, sr_ext_radius*1000);
    saturn_ring.drawable.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,1,0}, s_inclination);
    saturn_ring.drawable.uniform.shading = {1,0,0};
    // Load a texture (with transparent background)
    saturn_ring.drawable.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/saturn/8k_saturn_ring_alpha.png"), GL_REPEAT, GL_REPEAT );
}

void scene_model::setup_uranus()
{
    planet uranus;
    uranus = create_planet(u_radius, u_mass, u_p, u_v, u_inclination, u_force, u_orbitradius, u_vel_rot);
    uranus.drawable = mesh_primitive_sphere(u_radius*1000, {0,0,0}, 40, 80);
    uranus.drawable.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,1,0}, u_inclination);
    uranus.drawable.uniform.shading.specular = 0.0f;
    uranus.drawable.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/uranus/2k_uranus.png"));
    planets.push_back(uranus);
}

void scene_model::setup_neptune()
{
    planet neptune;
    neptune = create_planet(n_radius, n_mass, n_p, n_v, n_inclination, n_force, n_orbitradius, n_vel_rot);
    neptune.drawable = mesh_primitive_sphere(n_radius*1000, {0,0,0}, 40, 80);
    neptune.drawable.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,1,0}, n_inclination);
    neptune.drawable.uniform.shading.specular = 0.0f;
    neptune.drawable.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/neptune/2k_neptune.png"));
    planets.push_back(neptune);
}

void scene_model::setup_moon()
{
    moon = create_planet(mo_radius, mo_mass, mo_p, mo_v, mo_inclination, mo_force, mo_orbitradius, mo_vel_rot);
    moon.drawable = mesh_primitive_sphere(mo_radius*1000, {0,0,0}, 40, 80);
    moon.drawable.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,1,0}, mo_inclination);
    moon.drawable.uniform.shading.specular = 0.0f;
    moon.drawable.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/moon/8k_moon.png"));
}


// ************************** //
// DRAW FUNCTIONS
// ************************** //

void scene_model::draw_universe(std::map<std::string,GLuint>& shaders, scene_structure& scene)
{
    // Before displaying a textured surface: bind the associated texture id
    glBindTexture(GL_TEXTURE_2D, texture_universe_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    draw(universe, scene.camera, shaders["mesh"]);
    // After the surface is displayed it is safe to set the texture id to a white image
    // Avoids to use the previous texture for another object
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
}

void scene_model::draw_sun(std::map<std::string,GLuint>& shaders, scene_structure& scene)
{
    glBindTexture(GL_TEXTURE_2D, texture_sun_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    draw(sun.drawable, scene.camera, shaders["mesh"]);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
}

void scene_model::draw_planets(std::map<std::string,GLuint>& shaders, scene_structure& scene)
{
    for(planet& it : planets){
        glBindTexture(GL_TEXTURE_2D, it.drawable.texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        draw(it.drawable, scene.camera, shaders["mesh"]);
        glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    }
}

void scene_model::draw_moon(std::map<std::string,GLuint>& shaders, scene_structure& scene)
{
    glBindTexture(GL_TEXTURE_2D, moon.drawable.texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    draw(moon.drawable, scene.camera, shaders["mesh"]);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
}

void scene_model::draw_saturn_ring(std::map<std::string,GLuint>& shaders, scene_structure& scene)
{
    // Enable use of alpha component as color blending for transparent elements
    //  new color = previous color + (1-alpha) current color
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth buffer writing
    //  - Transparent elements cannot use depth buffer
    //  - They are supposed to be display from furest to nearest elements
    glDepthMask(false);

    glBindTexture(GL_TEXTURE_2D, saturn_ring.drawable.texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // avoids sampling artifacts

    draw(saturn_ring.drawable, scene.camera, shaders["mesh"]);

    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    glDepthMask(true);
}

void scene_model::draw_sun_ring(std::map<std::string,GLuint>& shaders, scene_structure& scene)
{
    // Enable use of alpha component as color blending for transparent elements
    //  new color = previous color + (1-alpha) current color
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth buffer writing
    //  - Transparent elements cannot use depth buffer
    //  - They are supposed to be display from furest to nearest elements
    glDepthMask(false);

    glBindTexture(GL_TEXTURE_2D, sun_ring.texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // avoids sampling artifacts

    sun_ring.uniform.transform.rotation = scene.camera.orientation;
    draw(sun_ring, scene.camera, shaders["mesh"]);

    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    glDepthMask(true);
}


// ************************** //
// UPDATE DATA FUNTIONS
// ************************** //

void scene_model::update_position_planets(float dt)
{
    for(planet& it : planets){
        vec3& p = it.p;
        vec3& v = it.v;

        vec3 F = it.force;

        // Numerical integration
        v = v + dt* F/it.mass;
        p = p + dt*v;

        const mat3 Inclination = rotation_from_axis_angle_mat3({0,1,0}, it.inclination);
        const mat3 Rotation = rotation_from_axis_angle_mat3({0,0,1}, it.hour);
        it.drawable.uniform.transform.rotation = Inclination * Rotation;
        it.drawable.uniform.transform.translation = p + back*normalize(p);
        it.force = G * sun.mass * it.mass/(norm(p)*norm(p)) * -1.0f *normalize(p);
        it.hour += it.vel_rot * dt;
    }
}

void scene_model::update_position_moon(float dt)
{
    vec3& mo_p = moon.p;
    vec3& mo_v = moon.v;

    vec3 mo_F = moon.force;

    // Numerical integration
    mo_v = mo_v + dt* mo_F/moon.mass;
    mo_p = mo_p + dt*mo_v;

    const mat3 Inclination = rotation_from_axis_angle_mat3({0,1,0}, moon.inclination);
    const mat3 Rotation = rotation_from_axis_angle_mat3({0,0,1}, moon.hour);
    moon.drawable.uniform.transform.rotation = Inclination * Rotation;
    moon.drawable.uniform.transform.translation = mo_p + back*normalize(mo_p) + 2000*planets[2].radius* normalize(mo_p - planets[2].p);
    moon.force = G * sun.mass * moon.mass/(norm(mo_p)*norm(mo_p)) * -1.0f *normalize(mo_p) + G * planets[2].mass * moon.mass/(norm(planets[2].p - mo_p)*norm(planets[2].p - mo_p)) * -1.0f *normalize(mo_p - planets[2].p);
    moon.hour += moon.vel_rot * dt;
}

void scene_model::update_position_saturn_ring()
{
    saturn_ring.drawable.uniform.transform.translation = planets[5].drawable.uniform.transform.translation;
    saturn_ring.drawable.uniform.transform.rotation = planets[5].drawable.uniform.transform.rotation;
}



 void scene_model::set_gui(timer_basic& timer)
 {
//     ImGui::SliderFloat("Time", &timer.t, timer.t_min, timer.t_max);
    ImGui::SliderFloat("Time scale", &timer.scale, 0.0f, 2.0f);

     ImGui::Text("Stars: "); ImGui::NewLine();

     //Planets
     ImGui::Checkbox("Mercury", &gui_scene.stars[0]); ImGui::NewLine();
     ImGui::Checkbox("Venus", &gui_scene.stars[1]); ImGui::NewLine();
     ImGui::Checkbox("Earth", &gui_scene.stars[2]); ImGui::SameLine();
     ImGui::Checkbox("Moon", &gui_scene.stars[9]); ImGui::NewLine();
     ImGui::Checkbox("Mars", &gui_scene.stars[3]); ImGui::NewLine();
     ImGui::Checkbox("Jupiter", &gui_scene.stars[4]); ImGui::NewLine();
     ImGui::Checkbox("Saturn", &gui_scene.stars[5]); ImGui::NewLine();
     ImGui::Checkbox("Uranus", &gui_scene.stars[6]); ImGui::NewLine();
     ImGui::Checkbox("Neptune", &gui_scene.stars[7]); ImGui::NewLine();
     ImGui::Checkbox("Sun", &gui_scene.stars[8]); ImGui::NewLine();


     ImGui::Text("Display: "); ImGui::SameLine();
     ImGui::Checkbox("Wireframe", &gui_scene.wireframe); ImGui::SameLine();

 }

 void scene_model::camera_position_at_each_star(scene_structure &scene){
     // Set the camera position at each star
     for (int i=0; i<10; i++){
         if (gui_scene.stars[i]){
             if(i<8){
                 scene.camera.translation = -planets[i].drawable.uniform.transform.translation;
                 break;
             }else if(i==8)
             {
                 scene.camera.translation = -sun.drawable.uniform.transform.translation;
                 break;
             }else if(i==9){
                 scene.camera.translation = -moon.drawable.uniform.transform.translation;
                 break;
             }
         }
     }
 }



