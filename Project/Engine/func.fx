#ifndef _FUNC
#define _FUNC

#include "value.fx"
#include "struct.fx"

int IsBinding(Texture2D _tex)
{
    int width = 0, height = 0;
    _tex.GetDimensions(width, height);
    
    if(0 == width || 0 == height)
    {
        return false;
    }
    
    return true;
}



void CalculateLight2D(int _LightIdx, float3 _WorldPos, inout tLight _Light)
{
    tLightInfo Info = g_Light2D[_LightIdx];
    
    // DirectionalLight 인 경우
    if(0 == Info.Type)
    {
        _Light.Color.rgb += Info.light.Color.rgb;
        _Light.Ambient.rgb += Info.light.Ambient.rgb;
    }
    
    // PointLight 인 경우
    else if(1 == Info.Type)
    {
        // 점광원과 픽셀까지의 거리
        float fDist = distance(Info.WorldPos.xy, _WorldPos.xy);
        
        // 거리값을 각도로 치환해서 거리에 따른 빛의 세기를 코사인 그래프 형태로 사용한다.
        float fPow = saturate(cos(saturate(fDist / Info.Radius) * (PI / 2.f)));
        
         // 광원으로부터 떨어진 거리에 따른 빛의 세기
        //float fPow2 = saturate(1.f - fDist / Info.Radius);
        
        // 최종 색상 계산 = 빛의 색 * 거리에따른 세기
        _Light.Color.rgb += Info.light.Color.rgb * fPow;
        _Light.Ambient.rgb += Info.light.Ambient.rgb;

    }
    
    // SpotLight 인 경우
    else
    {
        // 픽셀 위치로부터 광원까지의 거리
        float2 pixelDir = _WorldPos.xy - Info.WorldPos.xy;
        
        // SpotLight 방향 벡터
        float2 lightDir = normalize(pixelDir.xy);
        
        // 픽셀 위치부터 광원까지의 거리값
        float fDist = length(pixelDir); 
        
        // 두 벡터 사이의 각도 계산 (코사인 값)
        float angleCos = saturate(dot(lightDir.xy, Info.WorldDir.xy));

        
        if (angleCos > cos(Info.Angle) && fDist < Info.Radius)
        {
            _Light.Color.rgb += Info.light.Color.rgb;
        }
        
    }
}


void CalculateLight3D(int _LightIdx, float3 _ViewNormal, float3 _ViewPos, inout tLight _Light)
{
    tLightInfo LightInfo = g_Light3D[_LightIdx];
    
    float LightPow = 0.f;
    float SpecularPow = 0.f;
    float Ratio = 1.f;
    float SpecRatio = 1.f;
    
    // Directional Light
    if (0 == LightInfo.Type)
    {
        // VS 에서 받은 노말값으로, 빛의 세기를 PS 에서 직접 구한다음 빛의 세기를 적용
        float3 vLightDir = normalize(mul(float4(LightInfo.WorldDir, 0.f), matView).xyz);
        LightPow = saturate(dot(-vLightDir, _ViewNormal));
            
        // 반사광 계산
        // vR = vL + 2 * dot(-vL, vN) * vN;
        float3 vReflect = vLightDir + 2 * dot(-vLightDir, _ViewNormal) * _ViewNormal;
        vReflect = normalize(vReflect);
            
        // 카메라에서 물체를 향하는 vEye 를 구한다. 카메라는 원점에 있다.
        // 픽셀의 뷰스페이스 위치가 곧 카메라에서 물체를 향하는 Eye 방향이다.
        float3 vEye = normalize(_ViewPos);
    
        // 반사 방향과 시선 벡터를 내적해서 둘 사이의 벌어진 각도에 대한 cos 값을 반사광의 세기로 사용한다.
        SpecularPow = saturate(dot(vReflect, -vEye));
        SpecularPow = pow(SpecularPow, 15);
    }
    
    // Point Light
    else if (1 == LightInfo.Type)
    {
        // 표면 위치에서 광원의 위치를 뺀다. 광원에서 표면을 향하는 방향벡터를 구할 수 있다.
        float3 vLightViewPos = mul(float4(LightInfo.WorldPos, 1.f), matView).xyz;
        float3 vLightDir = normalize(_ViewPos - vLightViewPos);
        LightPow = saturate(dot(-vLightDir, _ViewNormal));
            
        // 반사광 계산
        // vR = vL + 2 * dot(-vL, vN) * vN;
        float3 vReflect = vLightDir + 2 * dot(-vLightDir, _ViewNormal) * _ViewNormal;
        vReflect = normalize(vReflect);
            
        // 카메라에서 물체를 향하는 vEye 를 구한다. 카메라는 원점에 있다.
        // 픽셀의 뷰스페이스 위치가 곧 카메라에서 물체를 향하는 Eye 방향이다.
        float3 vEye = normalize(_ViewPos);
    
        // 반사 방향과 시선 벡터를 내적해서 둘 사이의 벌어진 각도에 대한 cos 값을 반사광의 세기로 사용한다.
        SpecularPow = saturate(dot(vReflect, -vEye));
        SpecularPow = pow(SpecularPow, 15);
             
        // 거리에 따른 빛의 세기 감소량을 계산한다.
        float Distance = length(vLightViewPos - _ViewPos);
        float CamDistance = length(_ViewPos);
        //Ratio = saturate(1.f - (Distance / LightInfo.Radius));
        Ratio = saturate(cos((PI / 2.f) * saturate(Distance / LightInfo.Radius)));
        SpecRatio = saturate(cos((PI / 2.f) * saturate(CamDistance / LightInfo.Radius)));
    }
    
    // Spot Light
    else if (2 == LightInfo.Type)
    {
        // 표면 위치에서 광원의 위치를 뺀다. 광원에서 표면을 향하는 방향벡터를 구할 수 있다.
        float3 vLightViewPos = mul(float4(LightInfo.WorldPos, 1.f), matView).xyz;
        float3 vLightDir = normalize(_ViewPos - vLightViewPos);
    
    // 광원의 방향과 표면의 노말 벡터를 사용하여 광원의 세기를 계산한다.
        LightPow = saturate(dot(-vLightDir, _ViewNormal));

    // 스포트라이트의 방향과 표면 간의 각도 계산
        float3 spotDirection = normalize(mul(float4(LightInfo.WorldDir, 0.f), matView).xyz);
        float spotFactor = saturate(dot(-vLightDir, spotDirection));

    // 스포트라이트 각도 조건에 맞는지 확인
        if (spotFactor > cos(LightInfo.Angle))
        {
        // 거리 기반 감쇠 계산
            float Distance = length(vLightViewPos - _ViewPos);
            Ratio = saturate(cos((PI / 2.f) * saturate(Distance / LightInfo.Radius)));

        // 반사광 계산 (스페큘러 계산)
            float3 vReflect = vLightDir + 2 * dot(-vLightDir, _ViewNormal) * _ViewNormal;
            vReflect = normalize(vReflect);
            float3 vEye = normalize(_ViewPos);
            SpecularPow = saturate(dot(vReflect, -vEye));
            SpecularPow = pow(SpecularPow, 15);
        
        // 각도와 거리에 따른 세기 적용
            LightPow *= spotFactor; // 각도에 따른 세기 적용
        }
    }
      
    // 표면이 받는 빛 = 광원의 빛 * 표면이 받는 빛의 세기 * 거리에 따른 빛의 감소량
    _Light.Color += LightInfo.light.Color * LightPow * Ratio;
    _Light.Ambient += LightInfo.light.Ambient * Ratio;
    _Light.SpecCoef += LightInfo.light.SpecCoef * SpecularPow * SpecRatio;
}




float3 GetRandom(in Texture2D _NoiseTexture, uint _ID, uint _maxId)
{
    float2 vUV = (float2) 0.f;
    vUV.x = ((float) _ID / (float) (_maxId - 1)) + g_EngineTime * 0.5f;
    vUV.y = sin(vUV.x * 20 * PI) * 0.5f + g_EngineTime * 0.1f;
    float3 vRandom = _NoiseTexture.SampleLevel(g_sam_1, vUV, 0).xyz;
    
    return vRandom;
}



#endif