#pragma once

#include <dna/DNASequence.h>
#include <renderer/Mesh.h>
#include <vector>

namespace dna {

struct HelixParams {
    float radius { 1.5f  };  
    float rise { 0.34f };  
    float twist { 36.0f };  
    float tubeRadius { 0.15f };  
    float sphereRadius { 0.25f };  
    int tubeSides { 8 };  
    int sphereStacks { 8 };
};

struct HelixMeshes {
    Mesh strandA; 
    Mesh strandB; 
    Mesh bonds;      
};

class HelixGeometry {
public:
    static HelixMeshes build(const DNASequence& seq, const HelixParams& params = {});
    static glm::vec3 nucleotidePos(int index, const HelixParams& params, float phaseOffset);

private:
    static Mesh buildStrand(const DNASequence& seq, const HelixParams& params, float phaseOffset);
    static Mesh buildBonds(const DNASequence& seq, const HelixParams& params);
};

} // namespace dna