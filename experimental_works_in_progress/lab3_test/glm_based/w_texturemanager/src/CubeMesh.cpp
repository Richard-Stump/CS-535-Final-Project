#include "../include/CubeMesh.hpp"

CubeMesh::CubeMesh()
{

}

CubeMesh::~CubeMesh()
{
    
}

CubeMesh::CubeMesh(Vec3 _position)
{
    position = _position;
}

void CubeMesh::free_packet()
{
    packet2_free(packet);
}