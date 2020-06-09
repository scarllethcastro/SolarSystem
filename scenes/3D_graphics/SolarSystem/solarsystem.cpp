
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
planet& create_planet(float radius, float mass, vcl::vec3 p, vcl::vec3 v,  float inclination, vcl::vec3 force, float orbit_radius);
std::vector<vcl::vec3> trajectory (float a, float b);
mesh create_universe(float dimension);


/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_model::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& gui )
{
    // Create visual terrain surface

    // Setup initial camera mode and position
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 10.0f;
    scene.camera.apply_rotation(0,0,0,1.2f);



    // Data
//    float e_radius = 0.00064;
//    float e_mass = 0.005972;
//    vec3 e_p = {14.71, 0, 0,};
//    vec3 e_v = {0, 7.9704,0};
//    float e_inclination = 0;
//    float e_orbitradius = 14.96;
//    float sun_radius = 0.069634;
//    float sun_mass = 1989;
    vec3 e_force = 2000000*G * sun_mass * e_mass/(norm(e_p)*norm(e_p)) * -1.0f *normalize(e_p);

    // Universe creation
    universe = create_universe(50.0f);
    universe.uniform.shading = {1,0,0};

    // Stars creation
    // Sun
    sun = create_star(sun_radius, sun_mass, {0,0,0} , {0,0,0});
    sun.drawable = mesh_primitive_sphere(2.0f, {0,0,0}, 20, 40);
    //sun.drawable.uniform.color = {1.0f, 1.0f, 0.0f};
    sun.drawable.uniform.shading = {1,0,0};

    // Earth
    earth = create_planet(e_radius, e_mass, e_p, e_v, e_inclination, e_force, e_orbitradius);
    earth.drawable = mesh_primitive_sphere(1.0f, {0,0,0}, 20, 40);
    //earth.drawable.uniform.color = {0.0f, 0.0f, 1.0f};
    //earth.drawable.uniform.shading = {1,0,0};
    earth.drawable.uniform.shading.specular = 0.0f;

    // Textures
    // Universe
    texture_universe_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/8k_stars_milky.png"));

    // Sun
    texture_sun_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/8k_sun2.png"));

    // Earth
    texture_earth_id = create_texture_gpu( image_load_png("scenes/3D_graphics/SolarSystem/assets/8k_earth1.png"));
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
    float dt = timer.scale*0.01f;

    glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe

    // *** Earth data update *** //
    vec3& p = earth.p;
    vec3& v = earth.v;

    vec3 F = earth.force;

    // Numerical integration
    v = v + dt* F/earth.mass;
    p = p + dt*v;

    std::cout << "force = " << earth.force << std::endl;
    earth.drawable.uniform.transform.translation = p;
    earth.force = 2000000* G * sun.mass * earth.mass/(norm(p)*norm(p)) * -1.0f *normalize(p);

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

    // Earth
    glBindTexture(GL_TEXTURE_2D, texture_earth_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    draw(earth.drawable, scene.camera, shaders["mesh"]);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

//    earth.force = 2000000* G * sun.mass * earth.mass/(norm(p)*norm(p)) * -1.0f *normalize(p);

    if( gui_scene.wireframe ){ // wireframe if asked from the GUI
        glPolygonOffset( 1.0, 1.0 );
        draw(universe, scene.camera, shaders["wireframe"]);
        draw(sun.drawable,scene.camera, shaders["wireframe"]);
        draw(earth.drawable, scene.camera, shaders["wireframe"]);
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
// void scene_model::setup_mouvement_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
// {
//     // Initial Keyframe data vector of (position, time)
//     keyframes = { { {-1,1,0}   , 0.0f  },
//                   { {0,1,0}    , 1.0f  },
//                   { {1,1,0}    , 2.0f  },
//                   { {1,2,0}    , 2.5f  },
//                   { {2,2,0}    , 3.0f  },
//                   { {2,2,1}    , 3.5f  },
//                   { {2,0,1.5}  , 3.75f  },
//                   { {1.5,-1,1} , 4.5f  },
//                   { {1.5,-1,0} , 5.0f  },
//                   { {1,-1,0}   , 6.0f  },
//                   { {0,-0.5,0} , 7.0f },
//                   { {-1,-0.5,0}, 8.0f },
//                 };

//     // Set timer bounds
//     // You should adapt these extremal values to the type of interpolation
//     timer.t_min = keyframes[1].t;                   // first time of the keyframe
//     timer.t_max = keyframes[keyframes.size()-2].t;  // last time of the keyframe
//     timer.t = timer.t_min;

//     // Prepare the visual elements
//     const float radius_head = 0.12f;
//     const float a = 0.15f;
//     const float b = 0.1f;
//     const float c = 0.25f;
//     const float long_shoulder = 0.18f;
//     const float radius_cone = 0.05f;

//     // The geometry of the body is an ellipsoid
//     mesh_drawable body = mesh_drawable( mesh_primitive_ellipsoid(a, b, c, {0,0,0} ,40, 40));

//     // The geometry of the head is a sphere
//     mesh_drawable head = mesh_drawable( mesh_primitive_sphere(radius_head, {0,0,0} ,40, 40));

//     // Geometry of the eyes: black spheres
//     mesh_drawable eye = mesh_drawable(mesh_primitive_sphere(0.02f, {0,0,0}, 20, 20));
//     eye.uniform.color = {0,0,0};

//     // Shoulder part and arm are displayed as cylinder
//     mesh_drawable shoulder = mesh_primitive_quad({long_shoulder,0.0f,-0.1f},{0.0,0.0f,-0.1f},{0.0,0.0f,0.1f},{long_shoulder,0.0f,0.1f});
//     mesh_drawable arm = mesh_primitive_quad({0.1,0.0f,-0.04f}, {0.0f,0.0f,-0.1f}, {0.0f,0.0f,0.1f}, {0.1,0.0f,0.04f});

//     //Cone
//     mesh_drawable cone =  mesh_primitive_cone(radius_cone, {0,0,0},{0,0,0.15f}, 20, 10);
//     cone.uniform.color = {0.905f, 0.588f, 0.274f};



//     timer.scale = 0.5f;

//     keyframe_visual = mesh_primitive_sphere();
//     keyframe_visual.shader = shaders["mesh"];
//     keyframe_visual.uniform.color = {1,1,1};
//     keyframe_visual.uniform.transform.scaling = 0.05f;

//     keyframe_picked = mesh_primitive_sphere();
//     keyframe_picked.shader = shaders["mesh"];
//     keyframe_picked.uniform.color = {1,0,0};
//     keyframe_picked.uniform.transform.scaling = 0.055f;

//     segment_drawer.init();

//     trajectory = curve_dynamic_drawable(100); // number of steps stored in the trajectory
//     trajectory.uniform.color = {0,0,1};

//     picked_object=-1;
//     scene.camera.scale = 7.0f;
// }


// void scene_model::draw_mouvement_bodies(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
// {
//     timer.update();
//     const float t = timer.t;

//     if( t<timer.t_min+0.1f ) // clear trajectory when the timer restart
//         trajectory.clear();

//     set_gui();

//     // ********************************************* //
//     // Compute interpolated position at time t
//     // ********************************************* //
//     const int idx = index_at_value(t, keyframes);

//     // Assume a closed curve trajectory
//     const size_t N = keyframes.size();

//     // Preparation of data for the linear interpolation
//     // Parameters used to compute the linear interpolation
//     const float t0 = keyframes[idx-1].t; // = t_{i-1}
//     const float t1 = keyframes[idx  ].t; // = t_i
//     const float t2 = keyframes[idx+1].t; // = t_{i+1}
//     const float t3 = keyframes[idx+2].t; // = t_{i+2}

//     const vec3& p0 = keyframes[idx-1].p; // = p_{i-1}
//     const vec3& p1 = keyframes[idx  ].p; // = p_i
//     const vec3& p2 = keyframes[idx+1].p; // = p_{i+1}
//     const vec3& p3 = keyframes[idx+2].p; // = p_{i+2}

//     const float K = 0.5;
//     // Compute the linear interpolation here
//     const vec3 p = cardinal_spline_interpolation(t,t0,t1,t2,t3,p0,p1,p2,p3,K);
//     const vec3 dpdt = cardinal_spline_interpolation_speed(t,t0,t1,t2,t3,p0,p1,p2,p3,K);

//     // Create and call a function cardinal_spline_interpolation(...) instead
//     // ...

//     // Store current trajectory of point p
//     trajectory.add_point(p);
//     mat3 const R1 = rotation_between_vector_mat3({0,0,1}, {0,1,0});
//     mat3 const R2 = rotation_between_vector_mat3({0,-1,0}, {1,0,0});
//     const float norm_dpdt = norm(dpdt);
//     mat3 R_y = rotation_between_vector_mat3({1,0,0}, dpdt/norm_dpdt) * R2 *R1;
//     if(dot(R_y*vec3(0,0,1), vec3(0,0,1)) < 0){
//      mat3 const R = rotation_between_vector_mat3(R_y*vec3(0,0,1),-1.0f * R_y * vec3(0,0,1));
//        R_y = R * R_y;
//     }

//       hierarchy["body"].transform.rotation = R_y;

//     // Draw current position

//     hierarchy["body"].transform.translation = p;
//     draw_bird(scene, shaders);

//     // Draw moving point trajectory
//     trajectory.draw(shaders["curve"], scene.camera);


//     // Draw sphere at each keyframe position
//     if(gui_scene.display_keyframe) {
//         for(size_t k=0; k<N; ++k)
//         {
//             const vec3& p_keyframe = keyframes[k].p;
//             keyframe_visual.uniform.transform.translation = p_keyframe;
//             draw(keyframe_visual, scene.camera);
//         }
//     }

//     // Draw selected sphere in red
//     if( picked_object!=-1 )
//     {
//         const vec3& p_keyframe = keyframes[picked_object].p;
//         keyframe_picked.uniform.transform.translation = p_keyframe;
//         draw(keyframe_picked, scene.camera);
//     }


//     // Draw segments between each keyframe
//     if(gui_scene.display_polygon) {
//         for(size_t k=0; k<keyframes.size()-1; ++k)
//         {
//             const vec3& pa = keyframes[k].p;
//             const vec3& pb = keyframes[k+1].p;

//             segment_drawer.uniform_parameter.p1 = pa;
//             segment_drawer.uniform_parameter.p2 = pb;
//             segment_drawer.draw(shaders["segment_im"], scene.camera);
//         }
//     }

// }

star& create_star(float radius, float mass, vec3 p, vec3 v){
    static star new_star;
    new_star.mass = mass;
    new_star.radius = radius;
    new_star.p = p;
    new_star.v = v;

    return new_star;
}

planet& create_planet(float radius, float mass, vec3 p, vec3 v,  float inclination, vec3 force, float orbit_radius){
    static planet new_planet;
    new_planet.mass = mass;
    new_planet.radius = radius;
    new_planet.p = p;
    new_planet.v = v;

    new_planet.inclination = inclination;
    new_planet.force = force;
    new_planet.orbit_radius = orbit_radius;

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
//     ImGui::SliderFloat("Time scale", &timer.scale, 0.1f, 3.0f);

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

