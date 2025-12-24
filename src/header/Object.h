#include <vector>
#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <tiny_obj_loader.h>
#include "stb_image.h"

using namespace std;

enum class FACETYPE
{
	TRIANGLE,
	QUAD
};

class Object
{
public:
	vector<float> positions;
	vector<float> normals;
	vector<float> texcoords;
	vector<float> colors;  // Per-vertex colors from MTL
	FACETYPE faceType = FACETYPE::TRIANGLE;
	bool hasMaterials = false;

	void draw(){
		if(hasTexture){
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureID);
		}
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertex_cnt);
	}

	Object(const string& filename, bool loadMaterials = false)
	{
		loadOBJ(filename, loadMaterials);
		set_VAO();
	}

	void loadTexture(const string& filepath){
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
		unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			GLenum format = GL_RGB;
			if (nrChannels == 1) format = GL_RED;
			else if (nrChannels == 3) format = GL_RGB;
			else if (nrChannels == 4) format = GL_RGBA;

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			hasTexture = true;
		} else {
			std::cerr << "Failed to load texture: " << filepath << std::endl;
		}
		stbi_image_free(data);
	}

private:
	unsigned int VAO;
	unsigned int textureID = 0;
	bool hasTexture = false;
	int vertex_cnt;

	void addVertexData(const tinyobj::mesh_t& mesh, unsigned int idx, 
	                   const vector<tinyobj::material_t>& materials, int material_id) {
		// Positions
		if (idx * 3 + 2 < mesh.positions.size()) {
			positions.push_back(mesh.positions[idx * 3 + 0]);
			positions.push_back(mesh.positions[idx * 3 + 1]);
			positions.push_back(mesh.positions[idx * 3 + 2]);
		} else {
			positions.push_back(0.0f);
			positions.push_back(0.0f);
			positions.push_back(0.0f);
		}

		// Texture coordinates
		if (!mesh.texcoords.empty() && idx * 2 + 1 < mesh.texcoords.size()) {
			texcoords.push_back(mesh.texcoords[idx * 2 + 0]);
			texcoords.push_back(mesh.texcoords[idx * 2 + 1]);
		} else {
			texcoords.push_back(0.0f);
			texcoords.push_back(0.0f);
		}

		// Normals
		if (!mesh.normals.empty() && idx * 3 + 2 < mesh.normals.size()) {
			normals.push_back(mesh.normals[idx * 3 + 0]);
			normals.push_back(mesh.normals[idx * 3 + 1]);
			normals.push_back(mesh.normals[idx * 3 + 2]);
		} else {
			normals.push_back(0.0f);
			normals.push_back(1.0f);
			normals.push_back(0.0f);
		}

		// Colors from material
		if (hasMaterials && material_id >= 0 && material_id < (int)materials.size()) {
			colors.push_back(materials[material_id].diffuse[0]);
			colors.push_back(materials[material_id].diffuse[1]);
			colors.push_back(materials[material_id].diffuse[2]);
		} else {
			colors.push_back(1.0f);
			colors.push_back(1.0f);
			colors.push_back(1.0f);
		}
	}

	void loadOBJ(const string& filename, bool loadMaterials = false) {
		vector<tinyobj::shape_t> shapes;
		vector<tinyobj::material_t> materials;
		string err;

		// Get directory for MTL file
		string mtl_basepath = "";
		size_t last_slash = filename.find_last_of("/\\");
		if (last_slash != string::npos) {
			mtl_basepath = filename.substr(0, last_slash + 1);
		}

		bool ret = tinyobj::LoadObj(shapes, materials, err, filename.c_str(), mtl_basepath.c_str());

		if (!err.empty()) {
			cerr << "OBJ loading message: " << err << endl;
		}

		if (!ret) {
			cerr << "Failed to load OBJ file: " << filename << endl;
			return;
		}

		hasMaterials = loadMaterials && !materials.empty();
		
		if (hasMaterials) {
			cout << "Loaded " << materials.size() << " materials from MTL" << endl;
			for (size_t i = 0; i < materials.size(); i++) {
				cout << "  Material " << i << " (" << materials[i].name << "): Kd = " 
				     << materials[i].diffuse[0] << ", " 
				     << materials[i].diffuse[1] << ", " 
				     << materials[i].diffuse[2] << endl;
			}
		}

		// Process all shapes
		for (const auto& shape : shapes) {
			const tinyobj::mesh_t& mesh = shape.mesh;
			
			// Check if we have quads by examining face vertex counts
			for (size_t i = 0; i < mesh.num_vertices.size(); i++) {
				if (mesh.num_vertices[i] == 4) {
					faceType = FACETYPE::QUAD;
					break;
				}
			}

			// Process faces
			size_t index_offset = 0;
			for (size_t f = 0; f < mesh.num_vertices.size(); f++) {
				int fv = mesh.num_vertices[f];
				int material_id = -1;
				if (hasMaterials && f < mesh.material_ids.size()) {
					material_id = mesh.material_ids[f];
				}
				
				if (fv == 3) {
					// Triangle
					for (size_t v = 0; v < 3; v++) {
						unsigned int idx = mesh.indices[index_offset + v];
						addVertexData(mesh, idx, materials, material_id);
					}
				} else if (fv == 4) {
					// Quad - convert to two triangles
					vector<unsigned int> quad_indices;
					for (size_t v = 0; v < 4; v++) {
						quad_indices.push_back(mesh.indices[index_offset + v]);
					}

					// First triangle: 0, 1, 2
					for (int i : {0, 1, 2}) {
						addVertexData(mesh, quad_indices[i], materials, material_id);
					}

					// Second triangle: 0, 2, 3
					for (int i : {0, 2, 3}) {
						addVertexData(mesh, quad_indices[i], materials, material_id);
					}
				}
				
				index_offset += fv;
			}
		}
	}

	void set_VAO(){
		unsigned int VBO[4];
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glGenBuffers(4, VBO);

		// Positions (location 0)
		if (!positions.empty()) {
			glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * positions.size(), positions.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
			glEnableVertexAttribArray(0);
		}

		// Normals (location 1)
		if (!normals.empty()) {
			glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * normals.size(), normals.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
			glEnableVertexAttribArray(1);
		}

		// Texture coordinates (location 2)
		if (!texcoords.empty()) {
			glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * texcoords.size(), texcoords.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (void*)0);
			glEnableVertexAttribArray(2);
		}

		// Vertex colors (location 3)
		if (!colors.empty()) {
			glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * colors.size(), colors.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
			glEnableVertexAttribArray(3);
		}

		glBindVertexArray(0);

		vertex_cnt = positions.size() / 3;
		
		// Clear vectors to save memory after uploading to GPU
		positions.clear();
		texcoords.clear();
		normals.clear();
		colors.clear();
	}
};
