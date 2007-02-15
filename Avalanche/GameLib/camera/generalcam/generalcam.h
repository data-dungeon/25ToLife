///////////////////////////////////////////////////////////////////////////////
// A path following cam
///////////////////////////////////////////////////////////////////////////////
#ifndef GENERALCAM_H
#define GENERALCAM_H

#include "camera/camera.h"

class GeneralWTVC : public FlexTargetW2VC
{
	public:
		GeneralWTVC();
		virtual ~GeneralWTVC() {}

		// Setup the parameters
		virtual void		Setup(const Vector3 &Pos, const Vector3 &Target);

  		// Update 
		virtual void		Update(Camera &parent);

		// Focal point
		virtual Vector3	FocalPoint() { return m_focalPoint; }
		virtual void		SetFocalPoint( const Vector3 &v ) { m_focalPoint = v; }

		// Eye position
		Vector3	Eye() { return m_eye; }
		void		SetEye( const Vector3 &v ) { m_eye = v; }

	protected:
		Vector3 		m_eye;
		Vector3		m_focalPoint;
};

////////////////////////////////////////////////////////////////
// General projection controller
////////////////////////////////////////////////////////////////
class GeneralProjection : public DefaultProjectionController
{
public:
	GeneralProjection();
	~GeneralProjection() {}

  	// Update
	void Update(Camera &parent);
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
class GeneralCamera : public Camera
{
	public:

		enum CameraMode
		{
			CAMMODE_FOCAL_DIRECTION,
			CAMMODE_FOCAL_POINT
		};

		// Cameras must have at least a name and w2v controller
					GeneralCamera( const char *name , ViewportController *viewportC = NULL); 
					//GeneralCamera( const char *name , ProjectionController *projectionController = NULL , ViewportController *viewportController = NULL );
		virtual void Setup( const Vector3 &Pos , const Vector3 &Target ) { m_target.Setup( Pos , Target ); }

		void		RotateX(float radians) {}
		void		RotateY(float radians) {}

		void		SetDirection(const Vector3& v) { m_direction = v; }
		Vector3	&GetDirection( void ) { return m_direction; }
		virtual void SetFocalPoint( const Vector3& point ) { m_target.SetFocalPoint( (Vector3&)point ); }
		virtual void SetPosition( const Vector3& position ) { m_target.SetEye( (Vector3&)position ); }
		Vector3	&GetPosition( void ) { return m_position; }
		void		SetUp(const Vector3& up) { return; }

		void		SetHither( float hither ) { m_generalProj.SetHither( hither ); }
		void		SetFOV( float fov ) { m_generalProj.SetFOV( fov ); }
		float		GetFOV( void ) { return m_generalProj.FOV(); }

		int		GetMode( void ) { return d_mode; }
		void		SetMode( int mode ) { d_mode = mode; }

	protected:
		GeneralCamera( const char *name , WorldToViewController *worldToViewC, ProjectionController *projectionC = NULL , ViewportController *viewportC = NULL );

		int				d_mode;
		GeneralWTVC		m_target;
		GeneralProjection m_generalProj;
		Vector3			m_position;
		Vector3			m_direction;
};

#endif
