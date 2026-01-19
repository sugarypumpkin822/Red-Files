// Blood Effect Vertex Shader for DirectX
// Handles vertex transformation and setup for blood splatter effects

cbuffer TransformConstants : register(b0)
{
    matrix WorldMatrix;           // World transformation matrix
    matrix ViewMatrix;            // View transformation matrix
    matrix ProjectionMatrix;      // Projection transformation matrix
    matrix WorldViewProjection;   // Combined world-view-projection matrix
    float4x4 BoneTransforms[255]; // Bone transformation matrices for animation
    float Time;                   // Animation time
    float3 WindDirection;         // Wind direction for blood movement
    float WindStrength;           // Wind strength
    float2 UVScale;               // UV scaling factor
    float2 UVOffset;              // UV offset for animation
    float4 BloodFlowParams;       // Blood flow parameters (speed, direction, turbulence, viscosity)
    float4 DeformParams;          // Deformation parameters (amount, frequency, randomness, damping)
};

cbuffer MaterialConstants : register(b1)
{
    float4 MaterialColor;         // Base material color
    float Metallic;               // Metallic factor
    float Roughness;             // Roughness factor
    float Emission;               // Emission intensity
    float2 TextureScale;          // Texture scaling
    float2 TextureOffset;         // Texture offset
    float4 VertexColor;           // Vertex color multiplier
};

// Input vertex structure
struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD0;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    uint4 BoneIndices : BONEINDICES;
    float4 BoneWeights : BONEWEIGHTS;
    float3 InstancePosition : INSTANCE_POSITION;
    float4 InstanceRotation : INSTANCE_ROTATION;
    float3 InstanceScale : INSTANCE_SCALE;
    uint InstanceID : SV_InstanceID;
};

// Output structure to pixel shader
struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    float3 WorldPosition : WORLD_POSITION;
    float4 Color : COLOR;
    float2 BloodFlowUV : TEXCOORD1;
    float3 ViewDirection : TEXCOORD2;
    float4 ScreenPosition : TEXCOORD3;
    float3 WorldNormal : WORLD_NORMAL;
    float4 InstanceData : TEXCOORD4;
    float FogFactor : TEXCOORD5;
};

// Utility functions
float3 RotateVector(float3 vector, float4 rotation)
{
    // Rotate vector using quaternion
    float3 uv = cross(rotation.xyz, vector);
    uv = uv + rotation.xyz * dot(rotation.xyz, uv);
    float3 rotated = vector + uv * 2.0 * rotation.w + cross(rotation.xyz, uv) * 2.0;
    return rotated;
}

float4x4 CreateRotationMatrix(float4 rotation)
{
    // Create rotation matrix from quaternion
    float x = rotation.x;
    float y = rotation.y;
    float z = rotation.z;
    float w = rotation.w;
    
    float4x4 matrix = float4x4(
        1.0 - 2.0 * (y * y + z * z), 2.0 * (x * y - z * w), 2.0 * (x * z + y * w), 0.0,
        2.0 * (x * y + z * w), 1.0 - 2.0 * (x * x + z * z), 2.0 * (y * z - x * w), 0.0,
        2.0 * (x * z - y * w), 2.0 * (y * z + x * w), 1.0 - 2.0 * (x * x + y * y), 0.0,
        0.0, 0.0, 0.0, 1.0
    );
    
    return matrix;
}

float4x4 CreateScaleMatrix(float3 scale)
{
    // Create scale matrix
    return float4x4(
        scale.x, 0.0, 0.0, 0.0,
        0.0, scale.y, 0.0, 0.0,
        0.0, 0.0, scale.z, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

float4x4 CreateTranslationMatrix(float3 translation)
{
    // Create translation matrix
    return float4x4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        translation.x, translation.y, translation.z, 1.0
    );
}

float3 CalculateBloodDeformation(float3 position, float time, float3 worldPos)
{
    // Calculate blood surface deformation
    float3 deformation = float3(0.0, 0.0, 0.0);
    
    // Add wave-like deformation
    float waveFreq = DeformParams.y;
    float waveAmp = DeformParams.x;
    
    float wave1 = sin(position.x * waveFreq + time * BloodFlowParams.x) * waveAmp;
    float wave2 = cos(position.y * waveFreq * 1.3 + time * BloodFlowParams.x * 0.7) * waveAmp * 0.7;
    float wave3 = sin((position.x + position.y) * waveFreq * 0.8 + time * BloodFlowParams.x * 1.2) * waveAmp * 0.5;
    
    deformation.y += wave1 + wave2 + wave3;
    
    // Add turbulence
    float turbulence = sin(position.x * 10.0 + time * 5.0) * cos(position.y * 8.0 + time * 3.0);
    turbulence *= DeformParams.z * 0.1;
    deformation.y += turbulence;
    
    // Add wind effect
    float windEffect = dot(WindDirection, position) * WindStrength * 0.1;
    deformation.x += sin(windEffect + time) * DeformParams.x * 0.3;
    deformation.z += cos(windEffect + time * 0.7) * DeformParams.x * 0.2;
    
    // Apply damping
    deformation *= DeformParams.w;
    
    return deformation;
}

float2 CalculateBloodFlowUV(float2 baseUV, float3 worldPos, float time)
{
    // Calculate animated UV coordinates for blood flow effect
    float2 flowUV = baseUV;
    
    // Base flow animation
    flowUV += BloodFlowParams.xy * time * 0.1;
    
    // Add turbulence to flow
    float turbulence = sin(worldPos.x * 2.0 + time * 3.0) * cos(worldPos.y * 1.5 + time * 2.0);
    turbulence *= BloodFlowParams.z * 0.1;
    flowUV += float2(turbulence, turbulence * 0.7);
    
    // Add viscosity effect (slower flow in certain areas)
    float viscosity = sin(worldPos.x * 0.5) * cos(worldPos.y * 0.3);
    viscosity = saturate(viscosity * 0.5 + 0.5);
    flowUV *= (1.0 - viscosity * BloodFlowParams.w * 0.3);
    
    // Apply UV scaling and offset
    flowUV = flowUV * UVScale + UVOffset;
    
    return flowUV;
}

float3 CalculateAnimatedNormal(float3 normal, float3 position, float time)
{
    // Animate normal for dynamic lighting effects
    float3 animatedNormal = normal;
    
    // Add subtle normal animation
    float normalFreq = 2.0;
    float normalAmp = 0.1;
    
    animatedNormal.x += sin(position.x * normalFreq + time * 2.0) * normalAmp;
    animatedNormal.y += cos(position.y * normalFreq * 1.3 + time * 1.5) * normalAmp;
    animatedNormal.z += sin((position.x + position.y) * normalFreq * 0.8 + time * 1.8) * normalAmp * 0.5;
    
    return normalize(animatedNormal);
}

float4 CalculateVertexColor(float4 baseColor, float3 position, float time)
{
    // Calculate dynamic vertex color
    float4 vertexColor = baseColor;
    
    // Add pulsing effect
    float pulse = sin(time * 3.0) * 0.5 + 0.5;
    vertexColor.rgb *= (1.0 + pulse * 0.2);
    
    // Add position-based color variation
    float positionFactor = sin(position.x * 0.5) * cos(position.y * 0.3);
    vertexColor.rgb *= (1.0 + positionFactor * 0.1);
    
    // Apply material color
    vertexColor *= MaterialColor;
    
    return vertexColor;
}

float CalculateFogFactor(float3 worldPos, float3 cameraPos, float fogStart, float fogEnd)
{
    // Calculate fog factor for depth attenuation
    float distance = length(worldPos - cameraPos);
    float fogFactor = saturate((distance - fogStart) / (fogEnd - fogStart));
    return fogFactor;
}

// Bone animation function
float4x4 GetSkinningTransform(uint4 boneIndices, float4 boneWeights)
{
    // Calculate skinning transformation from bone weights
    float4x4 skinTransform = float4x4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    
    for (int i = 0; i < 4; i++)
    {
        if (boneWeights[i] > 0.0)
        {
            uint boneIndex = boneIndices[i];
            skinTransform += BoneTransforms[boneIndex] * boneWeights[i];
        }
    }
    
    return skinTransform;
}

// Main vertex shader
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    // Start with base position
    float3 position = input.Position;
    
    // Apply instance transformation
    float4x4 instanceMatrix = CreateTranslationMatrix(input.InstancePosition);
    float4x4 rotationMatrix = CreateRotationMatrix(input.InstanceRotation);
    float4x4 scaleMatrix = CreateScaleMatrix(input.InstanceScale);
    
    float4x4 instanceTransform = mul(scaleMatrix, mul(rotationMatrix, instanceMatrix));
    
    // Apply skinning if bones are used
    if (any(input.BoneWeights > 0.0))
    {
        float4x4 skinTransform = GetSkinningTransform(input.BoneIndices, input.BoneWeights);
        position = mul(float4(position, 1.0), skinTransform).xyz;
        input.Normal = normalize(mul(input.Normal, (float3x3)skinTransform));
        input.Tangent = normalize(mul(input.Tangent, (float3x3)skinTransform));
        input.Bitangent = normalize(mul(input.Bitangent, (float3x3)skinTransform));
    }
    
    // Apply instance transformation
    position = mul(float4(position, 1.0), instanceTransform).xyz;
    
    // Calculate world position
    float3 worldPos = mul(float4(position, 1.0), WorldMatrix).xyz;
    
    // Apply blood deformation
    float3 deformation = CalculateBloodDeformation(position, Time, worldPos);
    position += deformation;
    
    // Transform to world space
    float4 worldPosition = mul(float4(position, 1.0), WorldMatrix);
    output.WorldPosition = worldPosition.xyz;
    
    // Transform to view space
    float4 viewPosition = mul(worldPosition, ViewMatrix);
    
    // Transform to projection space
    output.Position = mul(viewPosition, ProjectionMatrix);
    output.ScreenPosition = output.Position;
    
    // Transform normals to world space
    float3x3 worldMatrix3x3 = (float3x3)WorldMatrix;
    output.Normal = normalize(mul(input.Normal, worldMatrix3x3));
    output.WorldNormal = output.Normal;
    output.Tangent = normalize(mul(input.Tangent, worldMatrix3x3));
    output.Bitangent = normalize(mul(input.Bitangent, worldMatrix3x3));
    
    // Animate normal for dynamic effects
    output.Normal = CalculateAnimatedNormal(output.Normal, worldPos, Time);
    
    // Pass through texture coordinates with animation
    output.TexCoord = input.TexCoord * TextureScale + TextureOffset;
    output.BloodFlowUV = CalculateBloodFlowUV(output.TexCoord, worldPos, Time);
    
    // Calculate vertex color
    output.Color = CalculateVertexColor(input.Color * VertexColor, worldPos, Time);
    
    // Calculate view direction
    float3 cameraPos = mul(float4(0, 0, 0, 1), ViewMatrix).xyz; // Camera position in world space
    output.ViewDirection = normalize(cameraPos - worldPos);
    
    // Store instance data for pixel shader
    output.InstanceData = float4(input.InstancePosition, input.InstanceID);
    
    // Calculate fog factor
    output.FogFactor = CalculateFogFactor(worldPos, cameraPos, 10.0, 100.0);
    
    return output;
}