macro(qterm_install_icons _theme)

   file(GLOB _icons *.png)
   file(GLOB _svg *svgz)
   set(_icons ${_icons} ${_svg})
   foreach(_current_ICON ${_icons} )
      get_filename_component(_ctype ${_current_ICON} EXT)

      if (${_ctype} STREQUAL ".png")
         string(REGEX REPLACE "^.*/[a-zA-Z]+([0-9]+)\\-([a-z]+)\\-(.+\\.png)$" "\\1" _size "${_current_ICON}")
         string(REGEX REPLACE "^.*/[a-zA-Z]+([0-9]+)\\-([a-z]+)\\-(.+\\.png)$" "\\2" _group "${_current_ICON}")
         string(REGEX REPLACE "^.*/[a-zA-Z]+([0-9]+)\\-([a-z]+)\\-(.+\\.png)$" "\\3" _name "${_current_ICON}")
         set(_sizestring "${_size}x${_size}")
      endif (${_ctype} STREQUAL ".png")

      if (${_ctype} STREQUAL ".svgz")
         string(REGEX REPLACE "^.*/[a-zA-Z]+\\-([a-z]+)\\-(.+\\.svgz)$" "\\1" _group "${_current_ICON}")
         string(REGEX REPLACE "^.*/[a-zA-Z]+\\-([a-z]+)\\-(.+\\.svgz)$" "\\2" _name "${_current_ICON}")
         set(_sizestring "scalable")
      endif (${_ctype} STREQUAL ".svgz")

      set(_icon_GROUP "actions")

      if(${_group} STREQUAL "mime")
         set(_icon_GROUP  "mimetypes")
      endif(${_group} STREQUAL "mime")

      if(${_group} STREQUAL "filesys")
         set(_icon_GROUP  "filesystems")
      endif(${_group} STREQUAL "filesys")

      if(${_group} STREQUAL "device")
         set(_icon_GROUP  "devices")
      endif(${_group} STREQUAL "device")

      if(${_group} STREQUAL "app")
         set(_icon_GROUP  "apps")
      endif(${_group} STREQUAL "app")

      if(${_group} STREQUAL "action")
         set(_icon_GROUP  "actions")
      endif(${_group} STREQUAL "action")

      set(_ICON_INSTALL_DIR share/icons/${_theme}/${_sizestring}/${_icon_GROUP})

      install(FILES ${_current_ICON} DESTINATION ${_ICON_INSTALL_DIR} RENAME ${_name})

   endforeach (_current_ICON)
endmacro(qterm_install_icons)

if (NOT APPLE)
  qterm_install_icons(hicolor)
endif ()
