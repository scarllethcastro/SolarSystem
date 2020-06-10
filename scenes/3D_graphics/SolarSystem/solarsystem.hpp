#pragma once

#include "main/scene_base/base.hpp"


#ifdef SCENE_SOLAR_SYSTEM

// Store a vec3 (p) + time (t)
struct vec3t{
    vcl::vec3 p; // position
    float t;     // time
};

// Stores some parameters that can be set from the GUI
struct gui_scene_structure
{
    bool display_keyframe = true;
    bool display_polygon  = true;
    bool wireframe   = false;
    bool surface     = true;
    bool skeleton    = false;
};

struct star {
    float radius;
    float mass;
    GLuint texture;
    vcl::mesh_drawable drawable;
    vcl::vec3 p ;
    vcl::vec3 v ;
};

struct planet: star {
    float inclination;
    vcl::vec3 force;
    float orbit_radius; // axis a
    float vel_rot;
    float hour =0;

};

struct scene_model : scene_base
{
    /** A part must define two functions that are called from the main function:
     * setup_data: called once to setup data before starting the animation loop
     * frame_draw: called at every displayed frame within the animation loop
     *
     * These two functions receive the following parameters
     * - shaders: A set of shaders.
     * - scene: Contains general common object to define the 3D scene. Contains in particular the camera.
     * - data: The part-specific data structure defined previously
     * - gui: The GUI structure allowing to create/display buttons to interact with the scene.
    */


    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void draw_mouvement_bodies(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void setup_mouvement_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);

    // Called every time the mouse is clicked
    void mouse_click(scene_structure& scene, GLFWwindow* window, int button, int action, int mods);
    // Called every time the mouse is moved
    void mouse_move(scene_structure& scene, GLFWwindow* window);



    void set_gui(vcl::timer_basic& timer);

    // visual representation of a surface
    gui_scene_structure gui_scene;
    GLuint texture_id;
    int picked_object;
    // Data (p_i,t_i)
    vcl::buffer<vec3t> keyframes; // Given (position,time)

    vcl::mesh_drawable point_visual;                       // moving point
    vcl::mesh_drawable keyframe_visual;                    // keyframe samples
    vcl::mesh_drawable keyframe_picked;
    // Store the index of a selected sphere

    vcl::hierarchy_mesh_drawable hierarchy;
    vcl::hierarchy_mesh_drawable_display_skeleton hierarchy_visual_debug;
    vcl::mesh_drawable ground;

    vcl::timer_interval timer;    // Timer allowing to indicate periodic events

    // Universe
    vcl::mesh_drawable universe;

    // Sun
    star sun;

    // Planets
    std::vector<planet> planets;
//    planet mercury;
//    planet earth;

    // Textures
    GLuint texture_universe_id;
    GLuint texture_sun_id;
//    GLuint texture_mercury_id;
//    GLuint texture_earth_id;

    // For debug, draw the trajectory that the planet should follow
    std::vector<vcl::vec3> trajectory;
};

#endif


