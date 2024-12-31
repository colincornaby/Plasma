//
//  Skinning.metal
//  plClient
//
//  Created by Colin Cornaby on 8/18/22.
//

#include <metal_stdlib>
using namespace metal;

struct SkinningUniforms {
    uint32_t destinationVerticesStride;
};

constant const int32_t numWeights [[ function_constant(0)    ]];
constant const bool hasSkinIndices [[ function_constant(1)    ]];

constant const bool hasWeight1 = numWeights > 0;
constant const bool hasWeight2 = numWeights > 1;
constant const bool hasWeight3 = numWeights > 2;

struct hsMatrix {
    float4x4 matrix;
    uint8_t      alignment[16];
};

struct VertexIn {
    float3 position [[ attribute(0) ]];
    float3 normal [[ attribute(1)] ];
    float weight1 [[ attribute(2), function_constant(hasWeight1)] ];
    float weight2 [[ attribute(3), function_constant(hasWeight2)] ];
    float weight3 [[ attribute(4), function_constant(hasWeight3)] ];
    uint32_t skinIndices [[ attribute(5), function_constant(hasSkinIndices) ]];
};

kernel void SkinningFunction(
                            VertexIn in [[stage_in]],
                            constant hsMatrix* matrixPalette [[buffer(1)]],
                            device char* destinationVertices [[buffer(2)]],
                            constant SkinningUniforms & uniforms [[buffer(3)]],
                            uint2 gid [[thread_position_in_grid]]
                            ) {
    float4 weights = {0};
    float weightSum = 0.f;
    for (uint8_t j = 0; j < numWeights; ++j) {
        weights[j] = (&in.weight1)[j];
        weightSum += weights[j];
    }
    weights[numWeights] = 1.f - weightSum;
    
    uint32_t indices;
    
    if (hasSkinIndices) {
        indices = in.skinIndices;
    } else
        indices = 1 << 8;
    
    float4 destNorm = { 0.f, 0.f, 0.f, 0.f };
    float4 destPoints = { 0.f, 0.f, 0.f, 1.f };
    
    for (int j = 0; j < numWeights + 1; ++j) {
        int index = indices & 0xFF;
        if (weights[j]) {
            const float4x4 matrix = matrixPalette[index].matrix;
            destPoints.xyz = in.position.xyz;
            destNorm.xyz = in.normal.xyz;
            //destPoints.xyz += weights[j] * (float4(in.position, 1.0f) * matrix).xyz;
            //destNorm.xyz += weights[j] * (float4(in.normal, 0.0f) * matrix).xyz;
        }
        indices >>= 8;
    }
    
    device packed_float3* dest = (device packed_float3*)(destinationVertices + (gid[0] * uniforms.destinationVerticesStride));
    dest[0] = destPoints.xyz;
    dest[1] = destNorm.xyz;
}
