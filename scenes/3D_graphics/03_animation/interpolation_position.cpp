
#include "interpolation_position.hpp"


#ifdef SCENE_INTERPOLATION_POSITION


using namespace vcl;


/** Function returning the index i such that t \in [v[i].t,v[i+1].t] */
static size_t index_at_value(float t, const vcl::buffer<vec3t> &v);

static vec3 linear_interpolation(float t, float t1, float t2, const vec3& p1, const vec3& p2);

static vec3 cardinal_spline_interpolation(float t, float t0, float t1, float t2, float t3, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3, float K);

mesh mesh_primitive_ellipsoid(float a, float b, float c, const vec3& p0, size_t Nu, size_t Nv);

mesh create_wing_hand(float l);

static vec3 speed_direction(float t, float t0, float t1, float t2, float t3, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3, float K);

void scene_model::setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    // Initial Keyframe data vector of (position, time)
    keyframes = { { {1,-1,0}   , 0.0f  },
                  { {0,1,0}    , 1.0f  },
                  { {1,1,0}    , 2.0f  },
                  { {1,2,0}    , 2.5f  },
                  { {2,2,0}    , 3.0f  },
                  { {2,2,1}    , 3.5f  },
                  { {2,0,1.5}  , 3.75f  },
                  { {1.5,-1,1} , 4.5f  },
                  { {1.5,-1,0} , 5.0f  },
                  { {1,-1,0}   , 6.0f  },
                  { /*{0,-0.5,0}*/{0,1,0} , 7.0f },
                  { {1,1,0}, 8.0f },
                };

    // Set timer bounds
    // You should adapt these extremal values to the type of interpolation
    timer.t_min = keyframes[1].t;                   // first time of the keyframe
    timer.t_max = keyframes[keyframes.size()-2].t;  // last time of the keyframe
    timer.t = timer.t_min;

    // Prepare the visual elements
    point_visual = mesh_primitive_sphere();
    point_visual.shader = shaders["mesh"];
    point_visual.uniform.color   = {0,0,1};
    point_visual.uniform.transform.scaling = 0.04f;

    keyframe_visual = mesh_primitive_sphere();
    keyframe_visual.shader = shaders["mesh"];
    keyframe_visual.uniform.color = {1,1,1};
    keyframe_visual.uniform.transform.scaling = 0.05f;

    keyframe_picked = mesh_primitive_sphere();
    keyframe_picked.shader = shaders["mesh"];
    keyframe_picked.uniform.color = {1,0,0};
    keyframe_picked.uniform.transform.scaling = 0.055f;

    segment_drawer.init();

    trajectory = curve_dynamic_drawable(100); // number of steps stored in the trajectory
    trajectory.uniform.color = {0,0,1};

    picked_object=-1;
    scene.camera.scale = 7.0f;

    create_bird(shaders, scene);
}




void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    timer.update();
    const float t = timer.t;

    if( t<timer.t_min+0.1f ) // clear trajectory when the timer restart
        trajectory.clear();

    set_gui();

    // ********************************************* //
    // Compute interpolated position at time t
    // ********************************************* //
    const int idx = index_at_value(t, keyframes);

    // Assume a closed curve trajectory
    const size_t N = keyframes.size();

    // Preparation of data for the linear interpolation
    // Parameters used to compute the linear interpolation
    const float t0 = keyframes[idx-1].t; // = t_{i-1}
    const float t1 = keyframes[idx  ].t; // = t_i
    const float t2 = keyframes[idx+1].t; // = t_{i+1}
    const float t3 = keyframes[idx+2].t; // = t_{i+2}

    const vec3& p0 = keyframes[idx-1].p; // = p_{i-1}
    const vec3& p1 = keyframes[idx  ].p; // = p_i
    const vec3& p2 = keyframes[idx+1].p; // = p_{i+1}
    const vec3& p3 = keyframes[idx+2].p; // = p_{i+2}



    // Compute the linear interpolation here
    //const vec3 p = linear_interpolation(t,t1,t2,p1,p2);

    // Create and call a function cardinal_spline_interpolation(...) instead
    const vec3 p = cardinal_spline_interpolation(t,t0,t1,t2,t3,p0,p1,p2,p3,0.1f);


    // Store current trajectory of point p
    trajectory.add_point(p);


    // Draw current position
    //point_visual.uniform.transform.translation = p;
    //draw(point_visual, scene.camera);
    hierarchy["body"].transform.translation = p;
    // Ajust the direction
    const vec3 direction_vector = speed_direction(t,t0,t1,t2,t3,p0,p1,p2,p3,0.1f);
    const mat3 direction_matrix = rotation_between_vector_mat3({0,0,1}, direction_vector);
    if(dot(direction_matrix*vec3(0,1,0),{0,0,1})<0){
        const vec3 correction_axis = direction_matrix*vec3(0,0,1);
        const mat3 correction = rotation_from_axis_angle_mat3(correction_axis, 3.14f);
        hierarchy["body"].transform.rotation = correction*direction_matrix;
    } else{
        hierarchy["body"].transform.rotation = direction_matrix;
    }

//    hierarchy["body"].transform.rotation = direction_matrix;
    draw_bird(shaders, scene);

    // Draw moving point trajectory
    //trajectory.draw(shaders["curve"], scene.camera);


    // Draw sphere at each keyframe position
    if(gui_scene.display_keyframe) {
        for(size_t k=0; k<N; ++k)
        {
            const vec3& p_keyframe = keyframes[k].p;
            keyframe_visual.uniform.transform.translation = p_keyframe;
            draw(keyframe_visual, scene.camera);
        }
    }

    // Draw selected sphere in red
    if( picked_object!=-1 )
    {
        const vec3& p_keyframe = keyframes[picked_object].p;
        keyframe_picked.uniform.transform.translation = p_keyframe;
        draw(keyframe_picked, scene.camera);
    }


    // Draw segments between each keyframe
    if(gui_scene.display_polygon) {
        for(size_t k=0; k<keyframes.size()-1; ++k)
        {
            const vec3& pa = keyframes[k].p;
            const vec3& pb = keyframes[k+1].p;

            segment_drawer.uniform_parameter.p1 = pa;
            segment_drawer.uniform_parameter.p2 = pb;
            segment_drawer.draw(shaders["segment_im"], scene.camera);
        }
    }

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
    const size_t N = keyframes.size();
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
        if(picked_object == 2)
            keyframes[N-1].p = p0;
        if(picked_object == N-1)
            keyframes[2].p = p0;
        if(picked_object == 1)
            keyframes[N-2].p = p0;
        if(picked_object == N-2)
            keyframes[1].p = p0;
        if(picked_object == 0)
            keyframes[N-3].p = p0;
        if(picked_object == N-3)
            keyframes[0].p = p0;
    }
}

void scene_model::set_gui()
{
    ImGui::SliderFloat("Time", &timer.t, timer.t_min, timer.t_max);
    ImGui::SliderFloat("Time scale", &timer.scale, 0.1f, 3.0f);

    ImGui::Text("Display: "); ImGui::SameLine();
    ImGui::Checkbox("keyframe", &gui_scene.display_keyframe); ImGui::SameLine();
    ImGui::Checkbox("polygon", &gui_scene.display_polygon);
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

    //ImGui::Spacing();
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


static vec3 linear_interpolation(float t, float t1, float t2, const vec3& p1, const vec3& p2)
{
    const float alpha = (t-t1)/(t2-t1);
    const vec3 p = (1-alpha)*p1 + alpha*p2;

    return p;
}

static vec3 cardinal_spline_interpolation(float t, float t0, float t1, float t2, float t3, const vec3 &p0, const vec3 &p1, const vec3 &p2, const vec3 &p3, float K)
{
    const float s = (t-t1)/(t2-t1);
    const vec3 d1 = 2*K*(p2-p0)/(t2-t0);
    const vec3 d2 = 2*K*(p3-p1)/(t3-t1);
    const float coef_p1 = 2*pow(s,3)-3*pow(s,2)+1;
    const float coef_d1 = pow(s,3)-2*pow(s,2)+s;
    const float coef_p2 = -2*pow(s,3)+3*pow(s,2);
    const float coef_d2 = pow(s,3)-pow(s,2);
    const vec3 p = coef_p1*p1 + coef_d1*d1 + coef_p2*p2 + coef_d2*d2;

    return p;
}

mesh mesh_primitive_ellipsoid(float a, float b, float c, const vec3& p0, size_t Nu, size_t Nv)
{
    mesh shape;
    for( size_t ku=0; ku<Nu; ++ku ) {
        for( size_t kv=0; kv<Nv; ++kv ) {

            const float u = static_cast<float>(ku)/static_cast<float>(Nu-1);
            const float v = static_cast<float>(kv)/static_cast<float>(Nv);

            const float theta = static_cast<float>( 3.14159f*u );
            const float phi = static_cast<float>( 2*3.14159f*v );

            const float x = a* std::sin(theta) * std::cos(phi);
            const float y = b * std::sin(theta) * std::sin(phi);
            const float z = c * std::cos(theta);

            const vec3 p = {x,y,z};
            const vec3 n = normalize(p);

            shape.position.push_back( p+p0 );
            shape.normal.push_back( n );
        }
    }
    shape.connectivity = connectivity_grid(Nu, Nv, false, true);

    return shape;
}

mesh create_wing_hand(float l)
{
    mesh shape;
    shape.position = {{0,0,-l/2.0f}, {-l*0.5f,0,-l/4.0f}, {-l*0.5f,0,l/4.0f}, {0,0,l/2.0f}};
    shape.connectivity = {{0,1,2}, {0,2,3}};
    return shape;
}

void scene_model::create_bird(std::map<std::string,GLuint>& shaders, scene_structure& scene)
{
    const float radius_head = 0.25f;
    const float radius_arm = 0.05f;
    const float length_arm = 0.2f;

    // Bird body
    mesh_drawable body = mesh_primitive_ellipsoid(0.4f, 0.3f, 0.55f, {0,0,0}, 50, 50);
    //body.uniform.color = {1,1,1};
    //body.shader = shaders["mesh"];

    // The geometry of the head is a sphere
    mesh_drawable head = mesh_drawable( mesh_primitive_sphere(radius_head, {0,0,0}, 40, 40));

    // "Mouth"
    mesh_drawable mouth = mesh_drawable( mesh_primitive_cone(0.125f,{0,0,0},{0,0,0.25f},30,30));
    mouth.uniform.color = {0.933f, 0.403f, 0.066f};

    // Wing
    const float l = 0.6f;
    mesh_drawable wing_arm = mesh_drawable(mesh_primitive_quad({0,0,-l/2.0f}, {-l*0.75f,0,-l/2.0f}, {-l*0.75f,0,l/2.0f}, {0,0,l/2.0f}));
    mesh_drawable wing_hand = create_wing_hand(l);

    // Geometry of the eyes: black spheres
    mesh_drawable eye = mesh_drawable(mesh_primitive_sphere(0.05f, {0,0,0}, 20, 20));
    eye.uniform.color = {0,0,0};

    // Shoulder part and arm are displayed as cylinder
    //mesh_drawable shoulder = mesh_primitive_cylinder(radius_arm, {0,0,0}, {-length_arm,0,0});
    //mesh_drawable arm = mesh_primitive_cylinder(radius_arm, {0,0,0}, {-length_arm/1.5f,-length_arm/1.0f,0});

    // An elbow displayed as a sphere
    //mesh_drawable elbow = mesh_primitive_sphere(0.055f);

    // Build the hierarchy:
    // Syntax to add element
    //   hierarchy.add(visual_element, element_name, parent_name, (opt)[translation, rotation])
    hierarchy.add(body, "body");
    hierarchy.add(head, "head", "body", {0, 0.25f, 0.6f});

    // Eyes positions are set with respect to some ratio of the
    hierarchy.add(eye, "eye_left", "head" , radius_head * vec3( 1/3.0f, 1/2.0f, 1/1.5f));
    hierarchy.add(eye, "eye_right", "head", radius_head * vec3(-1/3.0f, 1/2.0f, 1/1.5f));

    // Mouth position
    const float angle = 3.14f/10.0f;
    const vec3 mouth_translation = {0, -std::sin(angle) * radius_head * 0.8f, std::cos(angle) * radius_head * 0.8f};
    const mat3 mouth_rotation = rotation_from_axis_angle_mat3({1,0,0}, angle);
    hierarchy.add(mouth, "mouth", "head", {mouth_translation, mouth_rotation});

    // Right wing
    hierarchy.add(wing_arm, "wing_arm_right", "body", {-0.2f,0,0});
    hierarchy.add(wing_hand, "wing_hand_right", "wing_arm_right", {-l*0.75f,0,0});

    // Left wing
    hierarchy.add(wing_arm, "wing_arm_left", "body", {{0.2f,0,0}, {-1,0,0, 0,1,0, 0,0,1}/*Symmetry*/});
    hierarchy.add(wing_hand, "wing_hand_left", "wing_arm_left", {-l*0.75f,0,0});

    // Set the left part of the body arm: shoulder-elbow-arm
    //hierarchy.add(shoulder, "shoulder_left", "head", {-radius_head+0.05f,0,0}); // extremity of the spherical body
    //hierarchy.add(elbow, "elbow_left", "shoulder_left", {-length_arm,0,0});     // place the elbow the extremity of the "shoulder cylinder"
    //hierarchy.add(arm, "arm_bottom_left", "elbow_left");                        // the arm start at the center of the elbow

    // Set the right part of the body arm: similar to the left part excepted a symmetry is applied along x direction for the shoulder
    //hierarchy.add(shoulder, "shoulder_right", "head",     {{radius_head-0.05f,0,0}, {-1,0,0, 0,1,0, 0,0,1}/*Symmetry*/ } );
    //hierarchy.add(elbow, "elbow_right", "shoulder_right", {-length_arm,0,0});
    //hierarchy.add(arm, "arm_bottom_right", "elbow_right");


    // Set the same shader for all the elements
    hierarchy.set_shader_for_all_elements(shaders["mesh"]);


    // Initialize helper structure to display the hierarchy skeleton
    hierarchy_visual_debug.init(shaders["segment_im"], shaders["mesh"]);

    timer.scale = 0.5f;
}

void scene_model::draw_bird(std::map<std::string, GLuint> &shaders, scene_structure &scene)
{
    // Current time
    const float t = timer.t;

    /** *************************************************************  **/
    /** Compute the (animated) transformations applied to the elements **/
    /** *************************************************************  **/

    // The body oscillate along the z direction
    mat3 const head = rotation_from_axis_angle_mat3({1,0,0}, std::sin(2*3.14f*t)/3.0f);
    hierarchy["head"].transform.rotation = head;
    //hierarchy["body"].transform.translation = {0,0,0.2f*(1+std::sin(2*3.14f*t))};

    // Wings rotation
    mat3 const R_wing_arm = rotation_from_axis_angle_mat3({0,0,1}, 0.75f*std::sin(4*3.14f*(t-0.4f)) );
    mat3 const R_wing_hand = rotation_from_axis_angle_mat3({0,0,1}, 0.75f*std::sin(4*3.14f*(t-0.4f)) );

    // Rotation of the shoulder around the y axis
    //mat3 const R_shoulder = rotation_from_axis_angle_mat3({0,1,0}, std::sin(2*3.14f*(t-0.4f)) );
    // Rotation of the arm around the y axis (delayed with respect to the shoulder)
    //mat3 const R_arm = rotation_from_axis_angle_mat3({0,1,0}, std::sin(2*3.14f*(t-0.6f)) );
    // Symmetry in the x-direction between the left/right parts
    mat3 const Symmetry = {-1,0,0, 0,1,0, 0,0,1};

    // Set the rotation to the elements in the hierarchy
    //hierarchy["shoulder_left"].transform.rotation = R_shoulder;
    //hierarchy["arm_bottom_left"].transform.rotation = R_arm;

    //hierarchy["shoulder_right"].transform.rotation = Symmetry*R_shoulder; // apply the symmetry
    //hierarchy["arm_bottom_right"].transform.rotation = R_arm; //note that the symmetry is already applied by the parent element

    // Wings
    hierarchy["wing_arm_right"].transform.rotation = R_wing_arm;
    hierarchy["wing_hand_right"].transform.rotation = R_wing_hand;

    hierarchy["wing_arm_left"].transform.rotation = Symmetry*R_wing_arm;
    hierarchy["wing_hand_left"].transform.rotation = R_wing_hand;

    hierarchy.update_local_to_global_coordinates();


    /** ********************* **/
    /** Display the hierarchy **/
    /** ********************* **/

    if(gui_scene.surface) // The default display
        draw(hierarchy, scene.camera);

    if(gui_scene.wireframe) // Display the hierarchy as wireframe
        draw(hierarchy, scene.camera, shaders["wireframe"]);

    if(gui_scene.skeleton) // Display the skeleton of the hierarchy (debug)
        hierarchy_visual_debug.draw(hierarchy, scene.camera);

}

static vec3 speed_direction(float t, float t0, float t1, float t2, float t3, const vec3 &p0, const vec3 &p1, const vec3 &p2, const vec3 &p3, float K)
{
    const float s = (t-t1)/(t2-t1);
    const vec3 d1 = 2*K*(p2-p0)/(t2-t0);
    const vec3 d2 = 2*K*(p3-p1)/(t3-t1);

    const float coef_p1 = 6*pow(s,2)-6*s;
    const float coef_d1 = 3*pow(s,2)-4*s+1;
    const float coef_p2 = -6*pow(s,2)+6*s;
    const float coef_d2 = 3*pow(s,2)-2*s;
    const float dsdt = 1.0f/(t2-t1);

    const vec3 dpdt = (coef_p1*p1 + coef_d1*d1 + coef_p2*p2 + coef_d2*d2)*dsdt;

    return dpdt;
}

#endif

