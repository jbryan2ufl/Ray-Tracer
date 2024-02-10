#include "application.h"

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

application::application()
{
}

void application::init()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	// --------------------
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Display RGB Array", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(1);
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// // GLEW: load all OpenGL function pointers
	glewInit();

	glfwSwapInterval(1);

	// build and compile the shaders
	// ------------------------------------
	// vertex shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// link shaders
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	ioptr=&io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	

	float vertices[] = {
		// positions          // colors           // texture coords
		0.5f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		0.5f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {  
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// load and create a texture 
	// -------------------------
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// a.keyframes.push_back(std::make_pair(0.0f, glm::vec3{0, 2, -3}));
	// a.keyframes.push_back(std::make_pair(1.0f, glm::vec3{5, 2, 0}));
	// a.keyframes.push_back(std::make_pair(2.0f, glm::vec3{0, 2, 3}));
	// a.keyframes.push_back(std::make_pair(3.0f, glm::vec3{-5, 2, 0}));
	// a.keyframes.push_back(std::make_pair(4.0f, glm::vec3{0, 2, -3}));
}

void application::loop()
{
	// render loop
	// -----------

	while (!glfwWindowShouldClose(window))
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::SetNextWindowBgAlpha(1.0f);
		ImGui::SetNextWindowPos(ImVec2{SCR_WIDTH*3/4,0});
		ImGui::SetNextWindowSize(ImVec2{SCR_WIDTH*1/4,SCR_HEIGHT});

		ImGui::Begin("Settings", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration);
		{
			if (ImGui::CollapsingHeader("Camera Settings"))
			{
				ImGui::RadioButton("perspective", (int*)&rt.cam.ortho, 0);
				ImGui::SliderFloat("depth", &rt.cam.d, 0.5f, 3.0f);

				ImGui::RadioButton("orthographic", (int*)&rt.cam.ortho, 1);
				int res{glm::pow(2, rt.res_pow)};
				ImGui::Text("preview resolution: %ix%i", res, res);
				ImGui::SliderInt("##resolution", &rt.res_pow, 3, 7);
				ImGui::SameLine();
				if (ImGui::Button("Resize"))
				{
					rt.resize();
				}

				ImGui::SeparatorText("Camera Position");
				ImGui::Text("x: %f\ny: %f\nz: %f", rt.cam.e.x, rt.cam.e.y, rt.cam.e.z);
				ImGui::SeparatorText("Camera Basis");
				ImGui::Text("x: %f y: %f z: %f", rt.cam.u.x, rt.cam.u.y, rt.cam.u.z);
				ImGui::Text("x: %f y: %f z: %f", rt.cam.v.x, rt.cam.v.y, rt.cam.v.z);
				ImGui::Text("x: %f y: %f z: %f", rt.cam.w.x, rt.cam.w.y, rt.cam.w.z);
				ImGui::NewLine();
			}

			if (ImGui::CollapsingHeader("Materials"))
			{
				if (ImGui::Button("Add Material"))
				{
					rt.materials.push_back(new material{});
				}
				ImGui::SameLine();
				if (ImGui::Button("Remove Last"))
				{
					if (rt.materials.size() > 1)
					{
						delete rt.materials.back();
						rt.materials.pop_back();
					}
				}

				for (int i{}; i < rt.materials.size(); i++)
				{
					std::string str{std::to_string(i)};
					if (ImGui::TreeNode(("Material "+str).c_str()))
					{
						ImGui::Text("ambient :");
						ImGui::SameLine();
						ImGui::SliderFloat(("##matamb"+str).c_str(), &rt.materials[i]->k_a, 0, 1);
						ImGui::Text("diffuse :");
						ImGui::SameLine();
						ImGui::SliderFloat(("##matdiff"+str).c_str(), &rt.materials[i]->k_d, 0, 1);
						ImGui::Text("specular:");
						ImGui::SameLine();
						ImGui::SliderFloat(("##matspec"+str).c_str(), &rt.materials[i]->k_s, 0, 1);
						ImGui::Text("shine   :");
						ImGui::SameLine();
						ImGui::SliderInt(("##matshine"+str).c_str(), &rt.materials[i]->p, 1, 100);
						ImGui::Checkbox("Glazed", &rt.materials[i]->glazed);
						ImGui::NewLine();
						ImGui::TreePop();
					}
				}
				ImGui::NewLine();
			}
			
			if (ImGui::CollapsingHeader("Scene Objects"))
			{
				if (ImGui::Button("Add Sphere"))
				{
					rt.addSphere();
				}
				ImGui::SameLine();
				if (ImGui::Button("Add Triangle"))
				{
					rt.addTriangle();
				}
				ImGui::SameLine();
				if (ImGui::Button("Remove Last"))
				{
					if (rt.scene.size() > 0)
					{
						delete rt.scene.back();
						rt.scene.pop_back();
					}
				}

				for (int i{}; i < rt.scene.size(); i++)
				{
					std::string str{std::to_string(i)};
					ImGui::Checkbox(("##obj"+str).c_str(), &rt.scene[i]->visible);
					ImGui::SameLine();
					if (ImGui::TreeNode(dynamic_cast<sphere*>(rt.scene[i]) ? ("Sphere "+str).c_str() : ("Triangle "+str).c_str()))
					{
						if (dynamic_cast<sphere*>(rt.scene[i]))
						{
							ImGui::Text("color :");
							ImGui::SameLine();
							ImGui::ColorEdit3(("##sphere"+str).c_str(), (float*)&rt.scene[i]->color);
							ImGui::Text("center:");
							ImGui::SameLine();
							ImGui::SliderFloat3(("##sphere"+str).c_str(), (float*)&rt.scene[i]->center, -5, 5);
							ImGui::Text("radius:");
							ImGui::SameLine();
							ImGui::SliderFloat(("##sphere"+str).c_str(), (float*)&dynamic_cast<sphere*>(rt.scene[i])->r, 0.1, 5);
						}
						else
						{
							triangle* t = dynamic_cast<triangle*>(rt.scene[i]);
							ImGui::Text("plane :");
							ImGui::SameLine();
							ImGui::Checkbox("##tri", &t->plane);
							ImGui::Text("color :");
							ImGui::SameLine();
							ImGui::ColorEdit3(("##tri"+str).c_str(), (float*)&t->color);
							ImGui::Text("p1:");
							ImGui::SameLine();
							ImGui::SliderFloat3(("##tri1"+str).c_str(), (float*)&t->p1, -5, 5);
							ImGui::Text("p2:");
							ImGui::SameLine();
							ImGui::SliderFloat3(("##tri2"+str).c_str(), (float*)&t->p2, -5, 5);
							ImGui::Text("p3:");
							ImGui::SameLine();
							ImGui::SliderFloat3(("##tri3"+str).c_str(), (float*)&t->p3, -5, 5);
						}
						if (ImGui::BeginCombo("##material", ("Material "+std::to_string(rt.scene[i]->mat_idx)).c_str()))
						{
							for (int j{}; j < rt.materials.size(); j++)
							{
								const bool is_selected{rt.scene[i]->mat_idx==j};
								if (ImGui::Selectable(("Material "+std::to_string(j)).c_str(), is_selected))
								{
									rt.scene[i]->mat_idx=j;
									rt.scene[i]->m=rt.materials[j];
								}

								if (is_selected)
								{
									ImGui::SetItemDefaultFocus();
								}
							}
							ImGui::EndCombo();
						}
						ImGui::NewLine();
						ImGui::TreePop();
					}
				}
				ImGui::NewLine();
			}

			if (ImGui::CollapsingHeader("Light"))
			{
				if (ImGui::Button("Add Point Light"))
				{
					rt.point_lights.push_back(point_light{});
				}
				ImGui::SameLine();
				if (ImGui::Button("Remove Last"))
				{
					if (rt.point_lights.size() > 0)
					{
						rt.point_lights.pop_back();
					}
				}
				for (int i{}; i < rt.point_lights.size(); i++)
				{
					std::string str{std::to_string(i)};
					ImGui::Checkbox(("##light"+str).c_str(), &rt.point_lights[i].visible);
					ImGui::SameLine();
					if (ImGui::TreeNode(("Light "+str).c_str()))
					{
						ImGui::ColorEdit3(("##light"+str).c_str(), (float*)&rt.point_lights[i].color);
						ImGui::SliderFloat3(("##light"+str).c_str(), (float*)&rt.point_lights[i].p, -5, 5);
						ImGui::TreePop();
					}
				}
				ImGui::NewLine();
			}
			if (ImGui::CollapsingHeader("Shading"))
			{
				// ImGui::Text("blinn phong:");
				// ImGui::SameLine();
				ImGui::Checkbox("blinn phong", &rt.blinn_phong);
				ImGui::SliderInt("bounce count", &rt.bounce_count, 0, 5);
			}

			if (ImGui::CollapsingHeader("Export"))
			{
				int res{glm::pow(2, rt.export_res_pow)};
				ImGui::Text("export resolution: %ix%i", res, res);
				ImGui::SliderInt("##exportresolution", &rt.export_res_pow, 7, 11);
				if (ImGui::Button("Save"))
				{
					// save picture
					rt.export_image("Image.jpg");
				}
				ImGui::NewLine();
			}


		if (ImGui::Button("Play"))
		{
			freemove=!freemove;
			frameCount=0;
		}
		ImGui::Text("%i", keyframe_time);
		ImGui::SameLine();
		if (ImGui::Button("+##keyframe"))
		{
			keyframe_time++;
		}
		ImGui::SameLine();
		if (ImGui::Button("-##keyframe"))
		{
			keyframe_time--;
		}
		if (ImGui::Button("Add Frame"))
		{
			a.keyframes.push_back(std::make_pair(keyframe_time, rt.cam.e));
		}
		if (ImGui::Button("Remove All Frames"))
		{
			a.keyframes.clear();
		}

		ImGui::Text("%f", time);
		}
		ImGui::End();
		// ImGui::ShowDemoWindow();
		ImGui::Render();

		time = glfwGetTime();
		deltaTime = time - lastTime;
		// videoDeltaTime = time - lastTime;

		if (deltaTime < maxPeriod)
		{
			continue;
		}
		lastTime = time;

		if (!freemove)
		{
			// if (deltaTime < maxVideoPeriod)
			rt.cam.e=a.get_keyframe(videoTime);
			videoTime+=maxVideoPeriod;
			rt.lookat(rt.scene[0]->center);
			rt.export_image("frame_"+std::to_string(frameCount)+".jpg");
			frameCount++;
		}
		else
		{
			rt.update_image();
		}
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// bind Texture
		glBindTexture(GL_TEXTURE_2D, texture);

		// render container
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void application::close()
{
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void application::processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (freemove)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			rt.cam.e -= rt.cam.w*deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			rt.cam.e -= rt.cam.u*deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			rt.cam.e += rt.cam.w*deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			rt.cam.e += rt.cam.u*deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			rt.cam.w = glm::normalize(glm::vec3{rt.cam.w.x, rt.cam.w.y-deltaTime, rt.cam.w.z});
			rt.cam.u = glm::normalize(glm::cross(rt.cam.w, rt.cam.world_up));
			rt.cam.v = glm::normalize(glm::cross(rt.cam.u, rt.cam.w));
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			rt.cam.w = glm::normalize(glm::vec3{rt.cam.w.x, rt.cam.w.y+deltaTime, rt.cam.w.z});
			rt.cam.u = glm::normalize(glm::cross(rt.cam.w, rt.cam.world_up));
			rt.cam.v = glm::normalize(glm::cross(rt.cam.u, rt.cam.w));
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			rt.cam.w = glm::normalize(glm::vec3{rt.cam.w.x+deltaTime, rt.cam.w.y, rt.cam.w.z});
			rt.cam.u = glm::normalize(glm::cross(rt.cam.w, rt.cam.world_up));
			rt.cam.v = glm::normalize(glm::cross(rt.cam.u, rt.cam.w));
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			rt.cam.w = glm::normalize(glm::vec3{rt.cam.w.x-deltaTime, rt.cam.w.y, rt.cam.w.z});
			rt.cam.u = glm::normalize(glm::cross(rt.cam.w, rt.cam.world_up));
			rt.cam.v = glm::normalize(glm::cross(rt.cam.u, rt.cam.w));
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			rt.cam.e.y+=0.05f;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			rt.cam.e.y-=0.05f;
		}
	}
}

void application::process_keys(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		rt.cam.toggle_cam();
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void application::framebuffer_size(int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	application* obj = (application*)glfwGetWindowUserPointer(window);
	obj->process_keys(key, scancode, action, mods);

}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	application* obj = (application*)glfwGetWindowUserPointer(window);
	obj->framebuffer_size(width, height);
}