#ifndef LIGHTABLEMODEL_H
#define LIGHTABLEMODEL_H

#include "matma.h"

class LightableModel{
protected:
    Mat3 normal_matrix_;
    Mat3 normal_matrix_prim_;
    Mat3 normal_matrix_duo_;
};

#endif // LIGHTABLEMODEL_H
