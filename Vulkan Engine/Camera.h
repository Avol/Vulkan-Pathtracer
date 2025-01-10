#pragma once

#include <glm\glm.hpp>
#include <glm\geometric.hpp>
#include <glm\fwd.hpp>
#include <glm\gtx\rotate_vector.hpp>
#include <glm\gtx\matrix_decompose.hpp>

#include "src\Platform.h"
#include "src\Shared.h"

#include "src\Window.h"

#include <windows.h>


class Camera
{
	private:
		glm::mat4				_transform;

		glm::mat4x4				_projection;
		glm::mat4x4				_view;

		glm::mat4x4				_projection_view;
		glm::mat4x4				_inverse_projection_view;

		Window		*			_window;

		bool					_mouse_down;
		glm::vec2				_last_mouse_position;
	public:
		Camera( Window * window, glm::vec2 resolution );
		~Camera();

		bool					Update();

		glm::mat4x4             GetProjectionView();
		glm::mat4x4             GetInverseProjectionView();
};

