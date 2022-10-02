add_subdirectory(common/owlViewer)

# -------------------------------------------------------
# export those back to our parent
# -------------------------------------------------------
set(OWL_VIEWER_INCLUDES  ${OWL_VIEWER_INCLUDES}  PARENT_SCOPE)
set(OWL_VIEWER_LIBRARIES ${OWL_VIEWER_LIBRARIES} PARENT_SCOPE)
set(OWL_HAVE_TBB         ${OWL_HAVE_TBB}         PARENT_SCOPE)

