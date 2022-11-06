#pragma once

#include "../include/Shape.hpp"

void Shape::free_packet()
{
    packet2_free(packet);
}