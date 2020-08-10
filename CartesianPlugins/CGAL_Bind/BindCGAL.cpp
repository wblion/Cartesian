//
// Created by admin on 2020/8/4.
//
#define CGAL_NO_GMP 1

#include "BindCGAL.h"

// ----------------- Data exchange with glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../GLM_Bind/GLM_Matrix_Helper.h"
#include "../GLM_Bind/GLM_Vec_Helper.h"
#include "CartesianLog.h"
#include "BindCGAL_DefineType.h"


#include <vector>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <fstream>;


// ------------ add triangle by houdini------------------------
/*
int prim = addprim(geoself(),"poly");
addpoint(geoself(), set(0,1,0) );
addpoint(geoself(), set(1,1,0) );
addpoint(geoself(), set(0,0,0) );
addvertex(geoself(), prim, 0 );
addvertex(geoself(), prim, 1 );
addvertex(geoself(), prim, 2 );
 */



namespace Cartesian{

void BindCGAL::bind(sol::state *lua) {
    // bind vertex index
    sol::usertype<PRE_TYPE::Vertex_descriptor> vertex= (*lua).new_usertype<PRE_TYPE::Vertex_descriptor>(
            "vertex",
            sol::constructors<PRE_TYPE::Vertex_descriptor ,PRE_TYPE::Vertex_descriptor(int)>()
            );
    vertex["idx"] = &PRE_TYPE::Vertex_descriptor::idx;


    // bind face index
    sol::usertype<PRE_TYPE::Face_descriptor> face= (*lua).new_usertype<PRE_TYPE::Face_descriptor>(
            "face",
            sol::constructors<PRE_TYPE::Face_descriptor(),PRE_TYPE::Face_descriptor(int)>()
    );
    face["idx"] = &PRE_TYPE::Face_descriptor::idx;


    // bind half-edge index
    sol::usertype<PRE_TYPE::Halfedge_descriptor> half_edge= (*lua).new_usertype<PRE_TYPE::Halfedge_descriptor>(
            "half_edge",
            sol::constructors<PRE_TYPE::Halfedge_descriptor(),PRE_TYPE::Halfedge_descriptor(int)>()
    );
    half_edge["idx"] = &PRE_TYPE::Halfedge_descriptor::idx;


    // bind edge index
    sol::usertype<PRE_TYPE::Edge_descriptor> edge= (*lua).new_usertype<PRE_TYPE::Edge_descriptor>(
            "edge",
            sol::constructors<PRE_TYPE::Edge_descriptor(),PRE_TYPE::Edge_descriptor(int),PRE_TYPE::Edge_descriptor(PRE_TYPE::Halfedge_descriptor)>()
    );
    edge["idx"] = &PRE_TYPE::Edge_descriptor::idx;


    // bind mesh pointer
    sol::usertype<PRE_TYPE::Mesh> mesh= (*lua).new_usertype<PRE_TYPE::Mesh>(
            "mesh"
            );

    // todo addprim() need imp in DCC software,and return mesh handle.

    // mesh functions
    // addpoint(mesh, pt) : return vertex descriptor
    auto add_point = [](PRE_TYPE::Mesh & mesh, const glm::vec3 &ptpos){
        auto vd = mesh.add_vertex(PRE_TYPE::K::Point_3(ptpos.x, ptpos.y, ptpos.z));
        return vd;
    };
    auto add_point_table = [](PRE_TYPE::Mesh & mesh, const sol::lua_table & table){
        if(table.size() != 3){
            CARTESIAN_CORE_ERROR("addpoint(table) table length = 3");
            return PRE_TYPE::Vertex_descriptor(-1);
        }
        auto ptpos = GLM_Vec_Helper::table_to_vec3(table);
        auto vd = mesh.add_vertex(PRE_TYPE::K::Point_3(ptpos.x, ptpos.y, ptpos.z));
        return vd;
    };
    lua->set_function("addpoint",sol::overload(add_point,add_point_table));


    auto add_face_triangle = [](PRE_TYPE::Mesh& mesh, const PRE_TYPE::Vertex_descriptor &u, const PRE_TYPE::Vertex_descriptor &v, const PRE_TYPE::Vertex_descriptor &w) {
        return mesh.add_face(u, v, w);
    };
	auto add_face_quad = [](PRE_TYPE::Mesh& mesh, const PRE_TYPE::Vertex_descriptor& u, const PRE_TYPE::Vertex_descriptor& v, const PRE_TYPE::Vertex_descriptor& w, const PRE_TYPE::Vertex_descriptor &x) {
		return mesh.add_face(u, v, w,x);
	};
    lua->set_function("addface", sol::overload(add_face_triangle, add_face_quad));


    // mesh io save
    auto save_mesh = [](PRE_TYPE::Mesh& mesh, const std::string& path) {
		std::ofstream out(path);
		out << mesh;
    };
    lua->set_function("savemesh", save_mesh);


}

}
