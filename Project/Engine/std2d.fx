// C ��Ÿ���� #pragma once
#ifndef _TEST
#define _TEST

#include "value.fx"
#include "func.fx"

// Vertex Shader
struct VTX_IN
{
    float3 vPos         : POSITION;
    float4 vColor       : COLOR;
    float2 vUV          : TEXCOORD;
};

struct VTX_OUT
{
    float4 vPosition    : SV_Position;
    float4 vColor       : COLOR;
    float2 vUV          : TEXCOORD;
    
    float3 vWorldPos    : POSITION;
};


VTX_OUT VS_Std2D(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    // LocalSpace �� WorldSpace
    // float3 x float4x4(matrix)
    // float3 �� float4 �� ������ ���߾� �ش�.
    // ������ǥ�� 1 �� ����, ������� 4�࿡ ����ִ� �̵��� ����ްڴٴ� ��
    
    output.vPosition = mul(float4(_in.vPos, 1.f), matWVP);
    
    output.vColor = _in.vColor;
    output.vUV = _in.vUV;
    
    output.vWorldPos = mul(float4(_in.vPos, 1.f), matWorld).xyz;
    
    return output;
}


float4 PS_Std2D(VTX_OUT _in) : SV_Target
{
    float4 vColor = float4(0.f, 0.f, 0.f, 1.f);
    
    // FlipBook �� ����Ѵ�.
    if (UseFlipbook)
    {
        // _in.vUV : ��������Ʈ�� ������ ��ġ�� ������ ȯ���� ��
        float2 BackGroundLeftTop = LeftTopUV - (BackGroundUV - SliceUV) / 2.f;
        float2 vSpriteUV = BackGroundLeftTop + (_in.vUV * BackGroundUV);
        vSpriteUV -= OffsetUV;
        
        
        if (LeftTopUV.x <= vSpriteUV.x && vSpriteUV.x <= LeftTopUV.x + SliceUV.x
            && LeftTopUV.y <= vSpriteUV.y && vSpriteUV.y <= LeftTopUV.y + SliceUV.y)
        {
            vColor = g_AtlasTex.Sample(g_sam_1, vSpriteUV);
        }
        else
        {
           // vColor = float4(1.f, 1.f, 0.f, 1.f);
            discard;
        }
    }
    // FlipBook �� ������� �ʴ´�.
    else
    {
        if (g_btex_0)
        {
            vColor = g_tex_0.Sample(g_sam_1, _in.vUV);
        }
        else
        {
            vColor = float4(1.f, 0.f, 1.f, 1.f);
        }
    }

    if (vColor.a == 0.f)
    {
        //clip(-1);
        discard;
    }
    
    
    // ���� ����
    tLight Light = (tLight) 0.f;
    
    for (int i = 0; i < g_Light2DCount; ++i)
    {
        CalculateLight2D(i, _in.vWorldPos, Light);
    }
    
    vColor.rgb = vColor.rgb * Light.Color.rgb
               + vColor.rgb * Light.Ambient.rgb;

    return vColor;
}



//float4 PS_Std2D_Alphablend(VTX_OUT _in) : SV_Target
//{
//    float4 vColor = float4(0.f, 0.f, 0.f, 1.f);
    
//    if (g_btex_0)
//    {
//        vColor = g_tex_0.Sample(g_sam_1, _in.vUV);
//    }
//    else
//    {
//        vColor = float4(1.f, 0.f, 1.f, 1.f);
//    }
    
//    return vColor;
//}

float4 PS_Std2D_Alphablend(VTX_OUT _in) : SV_Target
{
    float4 vColor = float4(0.f, 0.f, 0.f, 1.f);
    
   // FlipBook �� ����Ѵ�.
    if (UseFlipbook)
    {
        // _in.vUV : ��������Ʈ�� ������ ��ġ�� ������ ȯ���� ��
        float2 BackGroundLeftTop = LeftTopUV - (BackGroundUV - SliceUV) / 2.f;
        float2 vSpriteUV = BackGroundLeftTop + (_in.vUV * BackGroundUV);
        vSpriteUV -= OffsetUV;
        
        
        if (LeftTopUV.x <= vSpriteUV.x && vSpriteUV.x <= LeftTopUV.x + SliceUV.x
            && LeftTopUV.y <= vSpriteUV.y && vSpriteUV.y <= LeftTopUV.y + SliceUV.y)
        {
            vColor = g_AtlasTex.Sample(g_sam_1, vSpriteUV);
        }
        else
        {
           // vColor = float4(1.f, 1.f, 0.f, 1.f);
            discard;
        }
    }
    // FlipBook �� ������� �ʴ´�.
    else
    {
        if (g_btex_0)
        {
            vColor = g_tex_0.Sample(g_sam_1, _in.vUV);
            
            float fAlpha = 1.f - saturate(dot(vColor.rb, vColor.rb) / 2.f);
            
            if(fAlpha < 0.1)
            {
                discard;
            }
        }
        else
        {
            vColor = float4(1.f, 0.f, 1.f, 1.f);
        }
    }

    //if (vColor.a == 0.f)
    //{
    //    //clip(-1);
    //    discard;
    //}
    
    
    // ���� ����
    tLight Light = (tLight) 0.f;
    
    for (int i = 0; i < g_Light2DCount; ++i)
    {
        CalculateLight2D(i, _in.vWorldPos, Light);
    }
    
    vColor.rgb = vColor.rgb * Light.Color.rgb
               + vColor.rgb * Light.Ambient.rgb;

    return vColor;
}


float4 PS_Std2D_UI(VTX_OUT _in) : SV_Target
{
    float4 vColor = float4(0.f, 0.f, 0.f, 1.f);
    
    if (g_btex_0)
    {
        vColor = g_tex_0.Sample(g_sam_1, _in.vUV);
    }
    else
    {
        vColor = float4(1.f, 0.f, 1.f, 1.f);
    }

    //if (_in.vUV.x > SliceUV.x)
    //{
    //    discard;
    //}
    //else
    //{
    //    vColor = float4(1.f, 0.f, 1.f, 1.f);
    //}


    if (vColor.a == 0.f)
    {
        //clip(-1);
        discard;
    }
    
    
    // ���� ����
    tLight Light = (tLight) 0.f;
    
    for (int i = 0; i < g_Light2DCount; ++i)
    {
        CalculateLight2D(i, _in.vWorldPos, Light);
    }
    
    vColor.rgb = vColor.rgb * Light.Color.rgb
               + vColor.rgb * Light.Ambient.rgb;

    
    return vColor;
}



float4 PS_Std2D_BOSSUI(VTX_OUT _in) : SV_Target
{
    float4 vColor = float4(0.f, 0.f, 0.f, 1.f);
    
    if (g_btex_0)
    {
        vColor = g_tex_0.Sample(g_sam_1, _in.vUV);
    }
    else
    {
        vColor = float4(1.f, 0.f, 1.f, 1.f);
    }

    //if (_in.vUV.x > SliceUV.x)
    //{
    //    discard;
    //}
    //else
    //{
    //    vColor = float4(1.f, 0.f, 1.f, 1.f);
    //}


    if (vColor.a == 0.f)
    {
        //clip(-1);
        discard;
    }
    
    
    // ���� ����
    tLight Light = (tLight) 0.f;
    
    for (int i = 0; i < g_Light2DCount; ++i)
    {
        CalculateLight2D(i, _in.vWorldPos, Light);
    }
    
    vColor.rgb = vColor.rgb * Light.Color.rgb
               + vColor.rgb * Light.Ambient.rgb;

    
    return vColor;
}



float4 PS_Std2D_HUD(VTX_OUT _in) : SV_Target
{
    float4 vColor = float4(0.f, 0.f, 0.f, 1.f);
    
    if (_in.vUV.x > HPRatio)
    {
        discard;
    }
    
    if (g_btex_0)
    {
        vColor = g_tex_0.Sample(g_sam_1, _in.vUV);
    }
  
    else
    {
        vColor = float4(1.f, 0.f, 1.f, 1.f);
    }


    if (vColor.a == 0.f)
    {
        //clip(-1);
        discard;
    }
    
    
    // ���� ����
    tLight Light = (tLight) 0.f;
    
    for (int i = 0; i < g_Light2DCount; ++i)
    {
        CalculateLight2D(i, _in.vWorldPos, Light);
    }
    
    vColor.rgb = vColor.rgb * Light.Color.rgb
               + vColor.rgb * Light.Ambient.rgb;

    
    return vColor;
}

float4 PS_Std2D_BOSSHUD(VTX_OUT _in) : SV_Target
{
    float4 vColor = float4(0.f, 0.f, 0.f, 1.f);
    
    if (_in.vUV.x > BOSSHPRatio)
    {
        discard;
    }
    
    if (g_btex_0)
    {
        vColor = g_tex_0.Sample(g_sam_1, _in.vUV);
    }
    else
    {
        vColor = float4(1.f, 0.f, 1.f, 1.f);
    }


    if (vColor.a == 0.f)
    {
        //clip(-1);
        discard;
    }
    
    
    // ���� ����
    tLight Light = (tLight) 0.f;
    
    for (int i = 0; i < g_Light2DCount; ++i)
    {
        CalculateLight2D(i, _in.vWorldPos, Light);
    }
    
    vColor.rgb = vColor.rgb * Light.Color.rgb
               + vColor.rgb * Light.Ambient.rgb;

    
    return vColor;
}



// ====================
//     Effect Shader
// ====================


VTX_OUT VS_Effect(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    // LocalSpace �� WorldSpace
    // float3 x float4x4(matrix)
    // float3 �� float4 �� ������ ���߾� �ش�.
    // ������ǥ�� 1 �� ����, ������� 4�࿡ ����ִ� �̵��� ����ްڴٴ� ��
    
    output.vPosition = mul(float4(_in.vPos, 1.f), matWVP);
    
    output.vColor = _in.vColor;
    output.vUV = _in.vUV;
    
    output.vWorldPos = mul(float4(_in.vPos, 1.f), matWorld).xyz;
    
    return output;
}


float4 PS_Effect(VTX_OUT _in) : SV_Target
{
    float4 vColor = float4(0.f, 0.f, 0.f, 0.f);
    
   // FlipBook �� ����Ѵ�.
    if (UseFlipbook)
    {
        // _in.vUV : ��������Ʈ�� ������ ��ġ�� ������ ȯ���� ��
        float2 BackGroundLeftTop = LeftTopUV - (BackGroundUV - SliceUV) / 2.f;
        float2 vSpriteUV = BackGroundLeftTop + (_in.vUV * BackGroundUV);
        vSpriteUV -= OffsetUV;
        
        
        if (LeftTopUV.x <= vSpriteUV.x && vSpriteUV.x <= LeftTopUV.x + SliceUV.x
            && LeftTopUV.y <= vSpriteUV.y && vSpriteUV.y <= LeftTopUV.y + SliceUV.y)
        {
            vColor = g_AtlasTex.Sample(g_sam_1, vSpriteUV);
        }
        else
        {
           // vColor = float4(1.f, 1.f, 0.f, 1.f);
            discard;
        }
    }
    // FlipBook �� ������� �ʴ´�.
    else
    {
        if (g_btex_0)
        {
            vColor = g_tex_0.Sample(g_sam_1, _in.vUV);
            
            //float fAlpha = 1.f - saturate(dot(vColor.rb, vColor.rb) / 2.f);
            
            //if (fAlpha < 0.1)
            //{
            //    discard;
            //}
        }
        //else
        //{
        //    discard;
        //}
    }


    
    vColor.rgb = g_vec4_0.xyz;
    
    return vColor;

}


#endif