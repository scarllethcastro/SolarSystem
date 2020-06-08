#pragma once

#include "main/scene_base/base.hpp"

#ifdef SCENE_3D_GRAPHICS

// Stores some parameters that can be set from the GUI
struct gui_scene_structure
{
    bool wireframe;
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

    void set_gui();

    // visual representation of a surface
    vcl::mesh_drawable terrain;

    // cylinder
    vcl::mesh_drawable cylinder;

    // cone
    vcl::mesh_drawable cone;

    // Positions des arbres
    std::vector<vcl::vec3> tree_position;

    // champignon
    vcl::mesh_drawable champ_cylinder;
    vcl::mesh_drawable champ_cone;

    // Positions des champignon
    std::vector<vcl::vec3> champignon_position;

    // Grass
    vcl::mesh_drawable grass;
    vcl::mesh_drawable strange_grass;

    // Grass position
    std::vector<vcl::vec3> grass_position;

    // Flower
    vcl::mesh_drawable flower;
    vcl::mesh_drawable strange_flower;

    //Flower position
    std::vector<vcl::vec3> flower_position;

    // Sky
    vcl::mesh_drawable sky;

    // Textures
    GLuint texture_terrain_id;
    GLuint texture_grass_id;
    GLuint texture_flower_id;
    GLuint texture_sky_id;

    gui_scene_structure gui_scene;
};

#endif


