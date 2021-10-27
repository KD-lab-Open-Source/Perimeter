
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

ADD_CUSTOM_TARGET(ScriptsCompile)
ADD_CUSTOM_TARGET(ScriptsCheck)

FOREACH(SCRIPT_PRM ${SCRIPTS_PRM})
    #We set the input .prm to process and the Source root -So arg 
    SET(XPRM_COMPILE_ARGS 
        "${PROJECT_SOURCE_DIR}/Source/Scripts/${SCRIPT_PRM}"
        "-So${PROJECT_SOURCE_DIR}/Source"
    )
    #Probably not necessary nowdays but apparently this was used for VS
    IF(MSVC_CL_BUILD)
        FILE(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/XPrm")
        SET(XPRM_COMPILE_ARGS ${XPRM_COMPILE_ARGS} "-Fo${CMAKE_BINARY_DIR}/XPrm/${SCRIPT_PRM}.bpr")
    ENDIF()

    #We set the -C arg, it has to use the same compile args
    SET(XPRM_CHECK_ARGS ${XPRM_COMPILE_ARGS} "-C")

    #Add this command as ScriptsCompile target dependency
    ADD_CUSTOM_COMMAND(
        TARGET ScriptsCompile PRE_BUILD
        COMMAND XPrm ${XPRM_COMPILE_ARGS}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Source
        VERBATIM
    )

    #Add this command as ScriptsCompile target dependency
    ADD_CUSTOM_COMMAND(
        TARGET ScriptsCheck PRE_BUILD
        COMMAND XPrm ${XPRM_CHECK_ARGS}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Source
        VERBATIM
    )
ENDFOREACH()
