
SET(SCRIPTS_PRM 
    "AI.prm"
    "Config.prm"
    "Filth.prm"
    "GameShellSq.prm"
    "InterfaceScriptExport.prm"
    "Mutation.prm"
    "Region.prm"
    "RigidBody.prm"
    "UnitAttribute.prm"
    "ForceField.prm"
    "Silicon.prm"
)

ADD_CUSTOM_TARGET(Scripts)

FOREACH(SCRIPT_PRM ${SCRIPTS_PRM})
    #We set the input .prm to process and the Source root -So arg 
    SET(XPRM_ARGS 
        "${PROJECT_SOURCE_DIR}/Source/Scripts/${SCRIPT_PRM}"
        "-So${PROJECT_SOURCE_DIR}/Source"
    )
    #Probably not necessary nowdays but apparently this was used for VS
    IF(MSVC_CL_BUILD)
        FILE(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/XPrm")
        SET(XPRM_ARGS ${XPRM_ARGS} "-Fo${CMAKE_BINARY_DIR}/XPrm/${SCRIPT_PRM}.bpr")
    ENDIF()
    
    #Add this command as Scripts target dependency
    ADD_CUSTOM_COMMAND(
        TARGET Scripts PRE_BUILD
        COMMAND XPrm ${XPRM_ARGS}
        VERBATIM
    )
ENDFOREACH()
