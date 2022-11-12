 #include "../globals/include/glmTexturedCube.hpp"

 void glm_create_local_world(glmTexturedCube* cube)
 {
 
  // Create the local_world matrix.
    Mat4 local_world = cube->model;
//   matrix_unit(local_world);
    local_world = glm::rotate(local_world, );
    matrix_rotate(local_world, local_world, rotation);
    matrix_translate(local_world, local_world, translation);
 
 }