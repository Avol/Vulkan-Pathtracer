#include "Camera.h"

Camera::Camera( Window * window, glm::vec2 resolution )
{
	_window						= window;

	glm::vec3 position			= glm::vec3(0.0f, 0.75f, -1.0f);

	_projection					= glm::perspective( 45.0f, resolution.x / resolution.y, 0.02f, 300.0f );
	_view						= glm::inverse( glm::lookAt( position, glm::vec3(0.0f, 0.75f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f) ) );

	_projection_view            = _projection * glm::inverse(_view);
	_inverse_projection_view    = glm::inverse( _projection_view );
}

Camera::~Camera()
{
}

glm::mat4x4 Camera::GetProjectionView()
{
	return _projection_view;
}

glm::mat4x4 Camera::GetInverseProjectionView()
{
	return _inverse_projection_view;
}

bool Camera::Update()
{
	const float speed = 0.05f;

	bool updated = false;

	glm::mat4 transform = glm::inverse( _view );

	glm::vec3 left		= glm::vec3(transform[0]);
	glm::vec3 up		= glm::vec3(transform[1]);
	glm::vec3 front		= glm::vec3(transform[2]);
	glm::vec3 position	= glm::vec3(transform[3]);

	glm::vec3 translate = glm::vec3(0);

	if (GetAsyncKeyState('A') & 0x8000)
	{
		translate = left * speed;
		updated = true;
	}
	else if (GetAsyncKeyState('D') & 0x8000)
	{
		translate = -left * speed;
		updated = true;
	}
	else if (GetAsyncKeyState('W') & 0x8000)
	{
		translate = front * speed;
		updated = true;
	}
	else if (GetAsyncKeyState('S') & 0x8000)
	{
		translate = -front * speed;
		updated = true;
	}

	if ((GetKeyState(VK_LBUTTON) & 0x100) != 0)
	{
		POINT p;
		if (GetCursorPos(&p))
		{
			ScreenToClient(_window->GetHandle(), &p);
			glm::vec2 mouse_position = glm::vec2(p.x, p.y);

			if (_mouse_down)
			{
				glm::vec2 diff		= _last_mouse_position - mouse_position;

				glm::vec3 rotY		= glm::normalize(front + up * (diff.y * 0.005f));
				glm::vec3 rotX		= glm::normalize(front + left * (diff.x * 0.005f));

				glm::vec3 up		= glm::vec3(0.0f, 1.0f, 0.0f);
				glm::vec3 target	= position - front - rotY - rotX;

				_view = glm::lookAt(position, target, up);
				_view = glm::translate(_view, translate);

				updated = true;
			}
			else
			{
				_view = glm::translate(_view, translate);
			}

			_last_mouse_position = mouse_position;
		}

		_mouse_down = true;
	}
	else
	{
		_mouse_down = false;
		_view = glm::translate(_view, translate);
	}


	_projection_view = _projection * _view;
	glm::mat4x4 new_proj_view_inverse = glm::inverse(_projection_view);
	new_proj_view_inverse == _inverse_projection_view ? updated = false : updated = true;
	_inverse_projection_view = new_proj_view_inverse;


	return updated;
}