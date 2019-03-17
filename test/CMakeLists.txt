
foreach ( testname eval  )
  add_executable( ${testname}_test src/${testname}.cpp )
  add_test( ${testname}_test ${testname}_test )

  target_link_libraries( ${testname}_test ${Boost_LIBRARIES} endhiran)
endforeach()
