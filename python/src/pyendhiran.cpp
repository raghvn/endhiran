/*
 * pyendhiran.cpp
 *
 *  Created on: 16-Nov-2018
 *      Author: raghav
 */

#include "board.h"
#include <boost/python.hpp>
#include <boost/python/list.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/extract.hpp>
# include <boost/python/suite/indexing/map_indexing_suite.hpp>

#undef USE_TT
#undef USE_KM


BOOST_PYTHON_MODULE(pyendhiran)
{

    using namespace boost::python;

    init_bitboards();
    Py_Initialize();


    class_<map_str_int_t >("map_str_int_t")
    .def(map_indexing_suite<map_str_int_t >() );

    Piece (Move::*get_promotion)() const = &Move::promotion;
    void (Move::*set_promotion)(Piece p)  = &Move::promotion;

    enum_<Color>("Color");

    class_<Move>("Move")
    .def("from", &Move::from)
    .def("to", &Move::to)
    .def("special", &Move::special)
    .def("piece", &Move::piece)
    .add_property("promotion",get_promotion,set_promotion)
    ;


    class_<Board>("Board", init<std::string>() )
        .def("init",&Board::init)
        .def("turn",&Board::turn)
        .def("readFen", &Board::readFen)
        .def(self_ns::str(self_ns::self))
        .def("think", &Board::think)
        .def("hashkey", &Board::hashkey)
        .def("eval", &Board::eval)
        .def("eval_features", &Board::eval_features)
        .def("hasMoves", &Board::hasMoves)
        .def("updateMove", &Board::updateMove)
        .def("perft", &Board::perft)
        .def("createMove", &Board::createMove)
        .def("bestMove", &Board::bestMove)
        .def("makeMove", &Board::makeMove)
        .def("unmakeMove", &Board::unmakeMove)
        .def("makeBestMove", &Board::makeBestMove)

    ;


};
