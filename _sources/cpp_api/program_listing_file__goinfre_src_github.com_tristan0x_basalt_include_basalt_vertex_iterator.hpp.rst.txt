
.. _program_listing_file__goinfre_src_github.com_tristan0x_basalt_include_basalt_vertex_iterator.hpp:

Program Listing for File vertex_iterator.hpp
============================================

|exhale_lsh| :ref:`Return to documentation for file <file__goinfre_src_github.com_tristan0x_basalt_include_basalt_vertex_iterator.hpp>` (``/goinfre/src/github.com/tristan0x/basalt/include/basalt/vertex_iterator.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   /*************************************************************************
    * Copyright (C) 2019 Blue Brain Project
    *
    * This file is part of Basalt distributed under the terms of the GNU
    * Lesser General Public License. See top-level LICENSE file for details.
    *************************************************************************/
   #pragma once
   
   #include <iterator>
   #include <memory>
   
   #include <basalt/fwd.hpp>
   
   namespace basalt {
   
   class VertexIterator: public std::iterator<std::input_iterator_tag, const vertex_uid_t> {
     public:
       template <EdgeOrientation Orientation>
       VertexIterator(const GraphImpl<Orientation>& pimpl, size_t from);
   
       VertexIterator(const VertexIterator& other);
   
       VertexIterator& operator++();
   
       const VertexIterator operator++(int);
   
       bool operator==(const VertexIterator& other) const;
   
       bool operator!=(const VertexIterator& other) const;
   
       const value_type& operator*();
   
       using VertexIteratorImpl_ptr = std::shared_ptr<VertexIteratorImpl>;
   
     private:
       VertexIteratorImpl_ptr pimpl_;
   };
   
   extern template VertexIterator::VertexIterator(
       const basalt::GraphImpl<EdgeOrientation::undirected>& pimpl,
       size_t from);
   extern template VertexIterator::VertexIterator(
       const basalt::GraphImpl<EdgeOrientation::directed>& pimpl,
       size_t from);
   
   }  // namespace basalt
