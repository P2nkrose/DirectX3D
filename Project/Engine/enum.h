#pragma once


enum class CB_TYPE
{
	TRANSFORM,		// �������ϴ� ��ü�� ��ġ����, ũ������
	MATERIAL,		// ����
	SPRITE,
	GLOBAL,			// ����Ѱ� �����ֱ�
	HUD,
	BOSSHUD,
	END,
};


enum class ASSET_TYPE
{
	MESH,
	MESH_DATA,
	MATERIAL,
	PREFAB,
	TEXTURE,
	SOUND,
	GRAPHIC_SHADER,
	COMPUTE_SHADER,
	SPRITE,
	FLIPBOOK,
	END,
};

extern const char* ASSET_TYPE_STRING[(UINT)ASSET_TYPE::END];
const char* ToString(ASSET_TYPE _Type);


enum class COMPONENT_TYPE
{
	TRANSFORM,
	COLLIDER2D,
	COLLIDER3D,
	LIGHT2D,
	LIGHT3D,
	FLIPBOOKCOMPONENT,
	ANIMATOR3D,
	STATEMACHINE,
	RIGIDBODY,
	CAMERA,

	MESHRENDER,
	TILEMAP,
	PARTICLESYSTEM,
	DECAl,
	SKYBOX,
	LANDSCAPE,

	END,

	SCRIPT,
};

extern const char* COMPONENT_TYPE_STRING[(UINT)COMPONENT_TYPE::END];
const char* ToString(COMPONENT_TYPE _Type);


enum DIR
{
	RIGHT,
	UP,
	FRONT,
};


enum class RS_TYPE
{
	CULL_BACK,
	CULL_FRONT,
	CULL_NONE,
	WIRE_FRAME,
	END,
};


enum class DS_TYPE
{
	LESS,					// Default��. ���̰� �� ������ ����
	LESS_EQUAL,				// ���̰� �۰ų� ���� �� ����

	GREATER,				// ���̰� �� �־�� ���� (3D���� ����)

	NO_TEST,				// ���������� ������, ������ �� ������Ŵ
	NO_WRITE,				// ���������� �ϰ�, �ڽ��� ���� ����� ���� �ʴ´�. (��ƼŬ �� ���)
	NO_TEST_NO_WRITE,		// ���������� �׻� �����Ű��, �ڽ��� ���� ��ϵ� ����

	END,
};


enum class BS_TYPE
{
	DEFAULT,		// �ڱ� ������ �״�� �׷���
	ALPHABLEND_COVERAGE,
	ALPHABLEND,		// ���İ��� �����Ű�� ���ĺ���
	ONE_ONE,		// 1:1�� �ջ������
	END,
};




enum SCALAR_PARAM
{
	INT_0,		INT_1,		INT_2,		INT_3,
	FLOAT_0,	FLOAT_1,	FLOAT_2,	FLOAT_3,
	VEC2_0,		VEC2_1,		VEC2_2,		VEC2_3,
	VEC4_0,		VEC4_1,		VEC4_2,		VEC4_3,
	MAT_0,		MAT_1,		MAT_2,		MAT_3,
};


enum TEX_PARAM
{
	TEX_0,
	TEX_1,
	TEX_2,
	TEX_3,
	TEX_4,
	TEX_5,

	TEXCUBE_0,
	TEXCUBE_1,

	TEXARR_0,
	TEXARR_1,

	END,
};


enum SHADER_DOMAIN
{
	DOMAIN_OPAQUE,		// ������
	DOMAIN_MASKED,		// ����, ������
	DOMAIN_TRANSPARENT, // ������
	DOMAIN_PARTICLE,	// ��ƼŬ
	DOMAIN_EFFECT,		// 2D ����Ʈ
	DOMAIN_POSTPROCESS,	// ��ó��
	DOMAIN_UI,			// UI
	DOMAIN_DEBUG,		// �����
	DOMAIN_NONE,		// �̼���
};


enum class DEBUG_SHAPE
{
	RECT,
	CIRCLE,
	LINE,
	CUBE,
	SPHERE,
};



enum TASK_TYPE
{
	// Param_0 : Layer, Param_1 : Object Adress
	CREATE_OBJECT,

	// Param_0 : Delete Object Adress
	DELETE_OBJECT,

	// Param_0 : Next Level State
	CHANGE_LEVELSTATE,

	// Param_0 : Level Adress, 
	CHANGE_LEVEL,

	// Param_0 : AssetAddress
	DEL_ASSET,

	// Param0 : Parent Object | Param1: Next State
	CHANGE_STATE,

	// Param0 : Collider Adress
	COLLIDER2D_SEMI_DEACTIVE,

	// Param0 : Collider Adress
	COLLIDER2D_DEACTIVE,

	LEVEL_CHANGED,
	ASSET_CHANGED,

	ADD_CHILD,
	COMPONENT_ACTIVATE,
	COMPONENT_DEACTIVATE,
};

enum LEVEL_STATE
{
	PLAY,
	PAUSE,
	STOP,
};


enum class LIGHT_TYPE
{
	DIRECTIONAL,
	POINT,
	SPOT,
};


enum class PARTICLE_MODULE
{
	SPAWN,
	SPAWN_BURST,
	ADD_VELOCITY,
	SCALE,
	DRAG,
	NOISE_FORCE,
	RENDER,

	END,
};


enum class STAGE_NAME
{
	TITLE,
	CHANGE,

	STAGE1,
	STAGE2,

	BOSS,

	END,
};


enum class LEVEL_TYPE
{
	TITLE,
	CHANGE,

	STAGE1,
	STAGE2,
	BOSS,

	END,
};


// ���� ���� 
// Left -1, Right 1, End 0
enum class DIRECTION
{
	LEFT = -1,
	RIGHT = 1,
	END = 0,
};
