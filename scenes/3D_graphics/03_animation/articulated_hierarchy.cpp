
#include "articulated_hierarchy.hpp"


#ifdef SCENE_ARTICULATED_HIERARCHY


using namespace vcl;

mesh mesh_primitive_ellipsoid(float a, float b, float c, const vec3 &p0, size_t Nu, size_t Nv);
mesh create_wing_hand(float l);

void scene_model::setup_data(std::map<std::string,GLuint>& shaders, scene_structure& , gui_structure& )
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




void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    timer.update();
    set_gui();

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

void scene_model::set_gui()
{
    ImGui::Text("Display: "); ImGui::SameLine();
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe); ImGui::SameLine();
    ImGui::Checkbox("Surface", &gui_scene.surface);     ImGui::SameLine();
    ImGui::Checkbox("Skeleton", &gui_scene.skeleton);   ImGui::SameLine();

    ImGui::Spacing();
    ImGui::SliderFloat("Time", &timer.t, timer.t_min, timer.t_max);
    ImGui::SliderFloat("Time scale", &timer.scale, 0.1f, 3.0f);

}





#endif

