

#include "solarsystem.hpp"

#include "data.hpp"

#include <cmath>

#ifdef SCENE_SOLAR_SYSTEM

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;



/** Function returning the index i such that t \in [v[i].t,v[i+1].t] */
static size_t index_at_value(float t, const vcl::buffer<vec3t> &v);

static vec3 linear_interpolation(float t, float t1, float t2, const vec3& p1, const vec3& p2);
static vec3 cardinal_spline_interpolation(float t, float t0, float t1, float t2, float t3, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3, float K);
static vec3 cardinal_spline_interpolation_speed(float t, float t0, float t1, float t2, float t3, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3, float K);
mesh mesh_primitive_ellipsoid(float a, float b, float c, const vec3 &p0, size_t Nu, size_t Nv);
star& create_star(float radius, float mass, vcl::vec3 p, vcl::vec3 v);
planet& create_planet(float radius, float mass, vcl::vec3 p, vcl::vec3 v,  float inclination, vcl::vec3 force, float orbit_radius, float vel_rot);
std::vector<vcl::vec3> trajectory (float a, float b);
mesh create_universe(float dimension);


/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_model::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& gui )
{
    std::cout << "G = " << G << std::endl;
    // Create visual terrain surface

    // Setup initial camera mode and position
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 25.0f;
    scene.camera.apply_rotation(0,0,0,1.2f);

    vec3 e_force = G * sun_mass * e_mass/(norm(e_p)*norm(e_p)) * -1.0f *normalize(e_p);

    // Universe creation
    universe = create_universe(1100.0f);
    universe.uniform.shading = {1,0,0};

    // Stars creation
    // Sun
    sun = create_star(sun_radius, sun_mass, {0,0,0} , {0,0,0});
    sun.drawable = mesh_primitive_sphere(sun_radius*100, {0,0,0}, 20, 40);
    sun.drawable.uniform.shading = {1,0,0};

    // Mercury
    planet mercury;
    mercury = create_planet(m_radius, m_mass, m_p, m_v, m_inclination, m_force, m_orbitradius, m_vel_rot);
    mercury.drawable = mesh_primitive_sphere(m_radius*1000, {0,0,0}, 20, 40);
    mercury.drawable.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,1,0}, m_inclination);
    mercury.drawable.uniform.shading.specular = 0.0f;
//    mercury.drawable.uniform.transform.translation = {10.0f,0,0};
    mercury.drawable.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/mercury/8k_mercury.png"));
    planets.push_back(mercury);

    // Venus
    planet venus;
    venus = create_planet(v_radius, v_mass,v_p, v_v, v_inclination, v_force, v_orbitradius, v_vel_rot);
    venus.drawable = mesh_primitive_sphere(v_radius*1000, {0,0,0}, 20, 40);
    venus.drawable.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,1,0}, v_inclination);
    venus.drawable.uniform.shading.specular = 0.0f;
    venus.drawable.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/venus/4k_venus.png"));
    planets.push_back(venus);

    // Earth
    planet earth;
    earth = create_planet(e_radius, e_mass, e_p, e_v, e_inclination, e_force, e_orbitradius, e_vel_rot);
    earth.drawable = mesh_primitive_sphere(e_radius*1000, {0,0,0}, 20, 40);
    earth.drawable.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,1,0}, e_inclination);
    earth.drawable.uniform.shading.specular = 0.0f;
    earth.drawable.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/earth/8k_earth.png"));
    planets.push_back(earth);

    // Mars
    planet mars;
    mars = create_planet(ma_radius, ma_mass, ma_p, ma_v, ma_inclination, ma_force, ma_orbitradius, ma_vel_rot);
    mars.drawable = mesh_primitive_sphere(ma_radius*1000, {0,0,0}, 20, 40);
    mars.drawable.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,1,0}, ma_inclination);
    mars.drawable.uniform.shading.specular = 0.0f;
    mars.drawable.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/mars/8k_mars.png"));
    planets.push_back(mars);

    // Jupiter
    planet jupiter;
    jupiter = create_planet(j_radius, j_mass, j_p, j_v, j_inclination, j_force, j_orbitradius, j_vel_rot);
    jupiter.drawable = mesh_primitive_sphere(j_radius*500, {0,0,0}, 20, 40);
    jupiter.drawable.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,1,0}, j_inclination);
    jupiter.drawable.uniform.shading.specular = 0.0f;
    jupiter.drawable.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/jupiter/8k_jupiter.png"));
    planets.push_back(jupiter);

    // Saturn
    planet saturn;
    saturn = create_planet(s_radius, s_mass, s_p, s_v, s_inclination, s_force, s_orbitradius, s_vel_rot);
    saturn.drawable = mesh_primitive_sphere(s_radius*500, {0,0,0}, 20, 40);
    saturn.drawable.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,1,0}, s_inclination);
    saturn.drawable.uniform.shading.specular = 0.0f;
    saturn.drawable.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/saturn/8k_saturn.png"));
    planets.push_back(saturn);

    // Uranus
    planet uranus;
    uranus = create_planet(u_radius, u_mass, u_p, u_v, u_inclination, u_force, u_orbitradius, u_vel_rot);
    uranus.drawable = mesh_primitive_sphere(u_radius*1000, {0,0,0}, 20, 40);
    uranus.drawable.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,1,0}, u_inclination);
    uranus.drawable.uniform.shading.specular = 0.0f;
    uranus.drawable.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/uranus/2k_uranus.png"));
    planets.push_back(uranus);

    // Neptune
    planet neptune;
    neptune = create_planet(n_radius, n_mass, n_p, n_v, n_inclination, n_force, n_orbitradius, n_vel_rot);
    neptune.drawable = mesh_primitive_sphere(n_radius*1000, {0,0,0}, 20, 40);
    neptune.drawable.uniform.transform.rotation = rotation_from_axis_angle_mat3({0,1,0}, n_inclination);
    neptune.drawable.uniform.shading.specular = 0.0f;
    neptune.drawable.texture_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/neptune/2k_neptune.png"));
    planets.push_back(neptune);

    // Textures
    // Universe
    texture_universe_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/universe/8k_stars_milky.png"));

    // Sun
    texture_sun_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/sun/8k_sun.png"));
}



/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure&)
{
 //   const float dt = timer.update();
    timer.update();
//    std::cout << "dt = " << dt << std::endl;
    set_gui(timer);

    // Simulation time step (dt)
    float dt = timer.scale*0.001f;

    glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe

    // *** Data update *** //
    // Using the list
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
        it.drawable.uniform.transform.translation = p;
        it.force = G * sun.mass * it.mass/(norm(p)*norm(p)) * -1.0f *normalize(p);
        it.hour += it.vel_rot * dt;
    }
    // ************************* //

    // *** Draw the elements *** //

    // Universe
    // Before displaying a textured surface: bind the associated texture id
    glBindTexture(GL_TEXTURE_2D, texture_universe_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    draw(universe, scene.camera, shaders["mesh"]);
    // After the surface is displayed it is safe to set the texture id to a white image
    // Avoids to use the previous texture for another object
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

    // Sun
    glBindTexture(GL_TEXTURE_2D, texture_sun_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    draw(sun.drawable, scene.camera, shaders["mesh"]);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

    // Planets
    for(planet& it : planets){
        glBindTexture(GL_TEXTURE_2D, it.drawable.texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        draw(it.drawable, scene.camera, shaders["mesh"]);
        glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    }

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
    }
    // ************************* //
}




 void scene_model::mouse_click(scene_structure& scene, GLFWwindow* window, int , int , int )
 {
     // Mouse click is used to select a position of the control polygon
     // ******************************************************************** //

     // Cursor coordinates
     const vec2 cursor = glfw_cursor_coordinates_window(window);

     // Check that the mouse is clicked (drag and drop)
     const bool mouse_click_left  = glfw_mouse_pressed_left(window);
     const bool key_shift = glfw_key_shift_pressed(window);

     // Check if shift key is pressed
     if(mouse_click_left && key_shift)
     {
         // Create the 3D ray passing by the selected point on the screen
         const ray r = picking_ray(scene.camera, cursor);

         // Check if this ray intersects a position (represented by a sphere)
         //  Loop over all positions and get the intersected position (the closest one in case of multiple intersection)
         const size_t N = keyframes.size();
         picked_object = -1;
         float distance_min = 0.0f;
         for(size_t k=0; k<N; ++k)
         {
             const vec3 c = keyframes[k].p;
             const picking_info info = ray_intersect_sphere(r, c, 0.1f);

             if( info.picking_valid ) // the ray intersects a sphere
             {
                 const float distance = norm(info.intersection-r.p); // get the closest intersection
                 if( picked_object==-1 || distance<distance_min ){
                     distance_min = distance;
                     picked_object = k;
                 }
             }
         }
     }

 }

 void scene_model::mouse_move(scene_structure& scene, GLFWwindow* window)
 {

     const bool mouse_click_left  = glfw_mouse_pressed_left(window);
     const bool key_shift = glfw_key_shift_pressed(window);
     if(mouse_click_left && key_shift && picked_object!=-1)
     {
         // Translate the selected object to the new pointed mouse position within the camera plane
         // ************************************************************************************** //

         // Get vector orthogonal to camera orientation
         const mat4 M = scene.camera.camera_matrix();
         const vec3 n = {M(0,2),M(1,2),M(2,2)};

         // Compute intersection between current ray and the plane orthogonal to the view direction and passing by the selected object
         const vec2 cursor = glfw_cursor_coordinates_window(window);
         const ray r = picking_ray(scene.camera, cursor);
         vec3& p0 = keyframes[picked_object].p;
         const picking_info info = ray_intersect_plane(r,n,p0);

         // translate the position
         p0 = info.intersection;

     }
 }

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

 void scene_model::set_gui(timer_basic& timer)
 {
//     ImGui::SliderFloat("Time", &timer.t, timer.t_min, timer.t_max);
    ImGui::SliderFloat("Time scale", &timer.scale, 0.0f, 2.0f);

     ImGui::Text("Display: "); ImGui::SameLine();
     ImGui::Checkbox("keyframe", &gui_scene.display_keyframe); ImGui::SameLine();
     ImGui::Checkbox("polygon", &gui_scene.display_polygon);
     ImGui::Text("Display: "); ImGui::SameLine();
     ImGui::Checkbox("Wireframe", &gui_scene.wireframe); ImGui::SameLine();
     ImGui::Checkbox("Surface", &gui_scene.surface);     ImGui::SameLine();
     ImGui::Checkbox("Skeleton", &gui_scene.skeleton);   ImGui::SameLine();

     if( ImGui::Button("Print Keyframe") )
     {
         std::cout<<"keyframe_position={";
         for(size_t k=0; k<keyframes.size(); ++k)
         {
             const vec3& p = keyframes[k].p;
             std::cout<< "{"<<p.x<<"f,"<<p.y<<"f,"<<p.z<<"f}";
             if(k<keyframes.size()-1)
                 std::cout<<", ";
         }
         std::cout<<"}"<<std::endl;
     }

 }



 static size_t index_at_value(float t, vcl::buffer<vec3t> const& v)
 {
     const size_t N = v.size();
     assert(v.size()>=2);
     assert(t>=v[0].t);
     assert(t<v[N-1].t);

     size_t k=0;
     while( v[k+1].t<t )
         ++k;
     return k;
 }

 std::vector<vec3> trajectory(float a, float b)
 {
     std::vector<vec3> trajectory;

     // Number of points
     size_t N = 100;
     float u = 2*3.14f/float(N);
     for(int i=0; i<N; i++){
         float theta = i*u;
         vec3 position = {a*std::cos(theta), b*std::sin(theta),0};
         trajectory.push_back(position);
     }

     return trajectory;
 }




#endif

