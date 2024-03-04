#ifndef MATERIALMODEL_H
#define MATERIALMODEL_H

#include <GL/glew.h>
#include "material.h"



class MaterialModel{
public:
    void SetMaterial(const Material & material){material_=material;}
    void SetMaterialPrim(const Material & material){material_prim_ = material;}
    void SetMaterialDuo(const Material & material){material_duo_ = material;}
protected:
    Material material_;
    Material material_prim_;
    Material material_duo_;
};

#endif // MATERIALMODEL_H
