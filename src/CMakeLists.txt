find_package(LibXml2 REQUIRED)
find_package(LibXslt REQUIRED)
find_package(Qt5DBus)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
include(ECMMarkAsTest)
include(ECMAddTests)
include(ECMAddAppIcon)
include(ECMInstallIcons)

include_directories( ${KDE5_INCLUDES} ${KDE5_INCLUDE_DIR} ${QT_INCLUDES}
   ${LIBXML2_INCLUDE_DIR} ${LIBXSLT_INCLUDE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/backends/
   ${CMAKE_CURRENT_SOURCE_DIR}/threadedqueries
   ${CMAKE_CURRENT_SOURCE_DIR}/importers
   ${CMAKE_CURRENT_SOURCE_DIR}/exporters
   ${CMAKE_CURRENT_SOURCE_DIR}/actionshandlers
   ${CMAKE_CURRENT_SOURCE_DIR}/tests
)

########### Deprecated warnings ###############

add_definitions (-DKRECIPES_DEPRECATED_WARNINGS)

############### dtos #################

set(krecipesdtos_SRCS
   dtos/kreelement.cpp
   dtos/kreingredient.cpp
   dtos/kreingheader.cpp
   )

############# backends ###############

set(sqlite_SRCS backends/SQLite/literecipedb.cpp)

set(mysql_SRCS backends/MySQL/mysqlrecipedb.cpp)

set(postgresql_SRCS backends/PostgreSQL/psqlrecipedb.cpp )

set(krecipesdbs_SRCS
   backends/recipedb.cpp
   backends/qsqlrecipedb.cpp
   backends/progressinterface.cpp
   backends/usda_property_data.cpp
   backends/usda_ingredient_data.cpp
   backends/usda_unit_data.cpp)

qt5_add_dbus_adaptor(krecipesdbs_SRCS backends/org.kde.krecipesdb.xml recipedb.h RecipeDB)

############## models ################

set(krecipesmodels_SRCS
   models/kreallingredientsmodels.cpp
   models/kreallingheadersmodels.cpp
   models/kresinglecolumnproxymodel.cpp
   )

############ importers ###############

set(krecipesimporters_SRCS
   importers/mx2importer.cpp
   importers/mmfimporter.cpp
   importers/mxpimporter.cpp
   importers/nycgenericimporter.cpp
   importers/recipemlimporter.cpp
   importers/baseimporter.cpp
   importers/kreimporter.cpp
   importers/rezkonvimporter.cpp
   importers/kredbimporter.cpp)

##### Threaded database queries  #####

set(krecipesthreadedqueries_SRCS
   threadedqueries/loadingauthorsthread.cpp
)

######### Actions Handlers ###########

set(krecipesactionshandlers_SRCS
   actionshandlers/krerecipeactionshandler.cpp
   actionshandlers/kregenericactionshandler.cpp
   actionshandlers/kreingredientactionshandler.cpp
   actionshandlers/kreheaderactionshandler.cpp
   actionshandlers/krepropertyactionshandler.cpp
   actionshandlers/kreunitactionshandler.cpp
   actionshandlers/kreprepmethodactionshandler.cpp
   actionshandlers/krecategoryactionshandler.cpp
   actionshandlers/kreauthoractionshandler.cpp

   #FIXME: The classes below are deprecated and must be removed sooner or later.
   actionshandlers/actionshandlerbase.cpp
   actionshandlers/recipeactionshandler.cpp
)

############### widgets ##############

set(krecipeswidgets_SRCS
   widgets/clickablelabel.cpp
   widgets/imagedroplabel.cpp
   widgets/krelistview.cpp
   widgets/kremenu.cpp
   widgets/paneldeco.cpp
   widgets/kreingredientlistwidget.cpp
   widgets/kreunitlistwidget.cpp
   widgets/krepropertylistwidget.cpp
   widgets/kreprepmethodlistwidget.cpp
   widgets/krecategorieslistwidget.cpp
   widgets/kreauthorlistwidget.cpp
   widgets/krerecipeslistwidget.cpp
   widgets/categorycombobox.cpp
   widgets/kretextedit.cpp
   widgets/kregenericlistwidget.cpp
   widgets/kresearchresultlistwidget.cpp
   widgets/kreconversiontable.cpp
   widgets/fractioninput.cpp
   widgets/ingredientcombobox.cpp
   widgets/headercombobox.cpp
   widgets/prepmethodcombobox.cpp
   widgets/inglistviewitem.cpp
   widgets/kdateedit.cpp
   widgets/kdatepickerpopup.cpp
   widgets/kreheaderlistwidget.cpp
   widgets/criteriacombobox.cpp
   widgets/unitcombobox.cpp
   widgets/unittypedelegate.cpp
   widgets/amountunitinput.cpp
   widgets/amountunitinput.cpp
   widgets/weightinput.cpp
   widgets/thumbbar.cpp

   #FIXME: The classes below are deprecated and must be removed sooner or later.
   #widgets/dblistviewbase.cpp #replaced by kregenericlistwidget.cpp
   #widgets/categorylistview.cpp #replaced by krecategorieslistwidget.cpp
   #widgets/propertylistview.cpp #replaced by krepropertylistwidget.cpp
   #widgets/ingredientlistview.cpp #replaced by kreingredientlistwidget.cpp

)

#qt5_wrap_ui(krecipeswidgets_SRCS
#   widgets/kregenericlistwidget.ui
#)

############## dialogs ###############

set(krecipesdialogs_SRCS

   #Setup assistant
   dialogs/setupassistant/setupassistant.cpp
   dialogs/setupassistant/welcomepage.cpp
   dialogs/setupassistant/dbtypesetuppage.cpp
   dialogs/setupassistant/sqlitesetuppage.cpp
   dialogs/setupassistant/permissionssetuppage.cpp
   dialogs/setupassistant/psqlpermissionssetuppage.cpp
   dialogs/setupassistant/serversetuppage.cpp
   dialogs/setupassistant/datainitializepage.cpp
   dialogs/setupassistant/savepage.cpp

#   dialogs/advancedsearchdialog.cpp
   dialogs/recipeimportdialog.cpp

   #Diet wizard
   dialogs/dietwizard/dietwizarddialog.cpp
   dialogs/dietwizard/dietviewdialog.cpp
   dialogs/dietwizard/mealinput.cpp
   dialogs/dietwizard/dishinput.cpp
   dialogs/dietwizard/dishtitle.cpp

   #Recipe input
   dialogs/recipeinput/recipeinputdialog.cpp
   dialogs/recipeinput/recipegeneralinfoeditor.cpp
   dialogs/recipeinput/selectauthorsdialog.cpp
   dialogs/recipeinput/resizerecipedialog.cpp
   dialogs/recipeinput/editratingdialog.cpp
   dialogs/recipeinput/ingredientparserdialog.cpp
   dialogs/recipeinput/selectcategoriesdialog.cpp
   dialogs/recipeinput/ingredientseditor.cpp
   dialogs/recipeinput/ingredientnamedelegate.cpp
   dialogs/recipeinput/amountdelegate.cpp
   dialogs/recipeinput/unitdelegate.cpp
   dialogs/recipeinput/prepmethoddelegate.cpp
   dialogs/recipeinput/nutrientinfostatuswidget.cpp
   dialogs/recipeinput/nutrientinfodetailsdialog.cpp
   dialogs/recipeinput/ratingdisplaywidget.cpp
   dialogs/recipeinput/ratinglisteditor.cpp

   dialogs/recipeviewdialog.cpp
   dialogs/selectrecipedialog.cpp
   dialogs/ingredientsdialog.cpp
   dialogs/selectunitdialog.cpp
   dialogs/createelementdialog.cpp
   dialogs/propertiesdialog.cpp
   dialogs/createpropertydialog.cpp
   dialogs/selectpropertydialog.cpp
   dialogs/unitsdialog.cpp
   dialogs/dependanciesdialog.cpp
   dialogs/shoppinglistdialog.cpp
   dialogs/shoppinglistviewdialog.cpp
   dialogs/categorieseditordialog.cpp
   dialogs/authorsdialog.cpp
   dialogs/ingredientmatcherdialog.cpp
   dialogs/prepmethodsdialog.cpp
   dialogs/createcategorydialog.cpp
   dialogs/borderdialog.cpp
   dialogs/refineshoppinglistdialog.cpp
   dialogs/pagesetupdialog.cpp
   dialogs/dbimportdialog.cpp
   dialogs/createunitdialog.cpp
   dialogs/setupdisplay.cpp
   dialogs/ingredientgroupsdialog.cpp
   dialogs/similarcategoriesdialog.cpp
   dialogs/conversiondialog.cpp
   dialogs/createingredientweightdialog.cpp
   dialogs/editpropertiesdialog.cpp
   dialogs/createunitconversiondialog.cpp

   #Preferences
   dialogs/preferences/krecipespreferences.cpp
   dialogs/preferences/sqliteserverprefs.cpp
   dialogs/preferences/mysqlserverprefs.cpp
   dialogs/preferences/postgresqlserverprefs.cpp
   dialogs/preferences/serverprefs.cpp
   dialogs/preferences/numberprefs.cpp
   dialogs/preferences/importprefs.cpp
   dialogs/preferences/performanceprefs.cpp
   dialogs/preferences/spellcheckingprefs.cpp
   )

#qt5_wrap_ui(krecipesdialogs_SRCS
#   dialogs/recipeinput/recipegeneralinfoeditor.ui
#   dialogs/recipeinput/resizerecipedialog.ui
#   dialogs/recipeinput/ratingdisplaywidget.ui
#   dialogs/recipeinput/ingredientseditor.ui
#   dialogs/recipeinput/nutrientinfostatuswidget.ui
#   dialogs/recipeinput/nutrientinfodetailsdialog.ui
#   )

qt5_add_resources(krecipesdialogs_SRCS
   ../pics/resources.qrc
   )


############# exporters ##############

set (krecipesexporters_SRCS
   exporters/kreexporter.cpp
   exporters/baseexporter.cpp
   exporters/cookmlexporter.cpp
   exporters/recipemlexporter.cpp
   exporters/mmfexporter.cpp
   exporters/htmlexporter.cpp
   exporters/plaintextexporter.cpp
   exporters/rezkonvexporter.cpp
   exporters/htmlbookexporter.cpp
   exporters/xsltexporter.cpp
   exporters/mx2exporter.cpp
   )

qt5_add_resources(krecipesexporters_SRCS
   ../pics/resources.qrc
   )

############ datablocks ##############

set(datablocks_SRCS
   datablocks/recipelist.cpp
   datablocks/constraintlist.cpp
   datablocks/categorytree.cpp
   datablocks/kreborder.cpp
   datablocks/recipe.cpp
   datablocks/ingredient.cpp
   datablocks/ingredientlist.cpp
   datablocks/elementlist.cpp
   datablocks/element.cpp
   datablocks/ingredientproperty.cpp
   datablocks/ingredientpropertylist.cpp
   datablocks/unit.cpp
   datablocks/unitratio.cpp
   datablocks/unitratiolist.cpp
   datablocks/mixednumber.cpp
   datablocks/mixednumberrange.cpp
   datablocks/rating.cpp
   datablocks/ratingcriteria.cpp
   datablocks/weight.cpp
   datablocks/yield.cpp
   )


############ validators ##############

set(validators_SRCS
   validators/mixednumbervalidator.cpp
   validators/mixednumberrangevalidator.cpp
   )


############### common library ################
# The reason to have this static library is to compile some files
# once for *all* executables in this project.
# (i.e. I want to save CPU time when compiling with test programs)

add_library(krecipescommon STATIC
   ${krecipesdtos_SRCS}
   ${mysql_SRCS} ${postgresql_SRCS} ${sqlite_SRCS} ${krecipesdbs_SRCS}
   ${krecipesmodels_SRCS}
   ${krecipesimporters_SRCS} ${krecipesexporters_SRCS}
   ${datablocks_SRCS} ${krecipesthreadedqueries_SRCS}
   ${validators_SRCS}
#   ${krecipesactionshandlers_SRCS}
#   ${krecipeswidgets_SRCS}
#   ${krecipesdialogs_SRCS}
   klomanager.cpp propertycalculator.cpp)


target_link_libraries(krecipescommon
   KF5::CoreAddons
   KF5::ConfigCore
   KF5::Archive
   KF5::KHtml
   KF5::Completion
   KF5::ItemModels
   KF5::IconThemes

   #${KDE5_KDECORE_LIBS} #${KDE4_KDE3SUPPORT_LIBS}
   Qt5::Sql
   Qt5::WebEngine
   Qt5::Test
   ${LIBXSLT_LIBRARIES} ${LIBXML2_LIBRARIES}
   ${QT_QTWEBKIT_LIBRARY} ${KDE5_KDEWEBKIT_LIBS}
   )

############### tests ################


set(IMPORT_EXPORT_TESTS
    kretest
    mmftest
    mx2test
    mxptest
    nyctest
    recipemltest
    rezkonvtest
    )

set(DATABLOCKS_TESTS
    rating
    ratingcriteria
    yield
    unit
    unitratio
    unitratiolist
    weight
    mixednumber
    mixednumberrange
    )

set(DATABASE_TESTS
    authors
    encoding
    )

set(UI_TESTS
#    ingredientseditor
    )

option(TESTS_MYSQL "Enable tests on MySQL" OFF)
option(TESTS_POSTGRESQL "Enable tests on PostgreSQL" OFF)

if(TESTS_MYSQL)
  add_definitions(-DKRE_TESTS_MYSQL)
endif()
if(TESTS_POSTGRESQL)
  add_definitions(-DKRE_TESTS_POSTGRESQL)
endif()

macro(add_import_export_test _TEST)
    if(KDE5_BUILD_TESTS)
        kde5_add_executable(krecipes-import-export-${_TEST}-test tests/import-export/${_TEST}.cpp)
        target_link_libraries(krecipes-import-export-${_TEST}-test ${UNIT_TEST_LIBRARIES})

        set(_TEST_DATA
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/import-export/${_TEST}.txt
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/import-export/test_photo.jpg)

        add_test(NAME krecipes-import-export-${_TEST}-test
            COMMAND $<TARGET_FILE:krecipes-import-export-${_TEST}-test> ${_TEST_DATA})
    endif(KDE5_BUILD_TESTS)
endmacro(add_import_export_test)

macro(add_datablocks_test _TEST)
    add_executable(krecipes-datablocks-${_TEST}-test tests/datablocks/${_TEST}.cpp)
    add_test(krecipes-datablocks krecipes-datablocks-${_TEST}-test)
    ecm_mark_as_test(krecipes-datablocks-${_TEST}-test)
    target_link_libraries(krecipes-datablocks-${_TEST}-test ${UNIT_TEST_LIBRARIES})
endmacro(add_datablocks_test)

macro(add_database_test _TEST)
    set(krecipes-database-${_TEST}_SRCS
        tests/database/${_TEST}.cpp)
    qt5_add_resources(krecipes-database-${_TEST}_SRCS
        tests/database/dbconfigs.qrc)
    add_executable(krecipes-database-${_TEST}-test ${krecipes-database-${_TEST}_SRCS})
    add_test(krecipes-database krecipes-database-${_TEST}-test)
    ecm_mark_as_test(krecipes-database-${_TEST}-test)
    target_link_libraries(krecipes-database-${_TEST}-test ${UNIT_TEST_LIBRARIES})
endmacro(add_database_test)

macro(add_ui_test _TEST)
    add_executable(krecipes-ui-${_TEST}-test tests/ui/${_TEST}.cpp)
    add_test(krecipes-ui krecipes-ui-${_TEST}-test)
    ecm_mark_as_test(krecipes-ui-${_TEST}-test)

    target_link_libraries(krecipes-ui-${_TEST}-test ${UNIT_TEST_LIBRARIES})
endmacro(add_ui_test)

set(UNIT_TEST_LIBRARIES krecipescommon
#    KF5::KDELibs4Support
    ${QT_QTSQL_LIBRARY}  ${QT_QTTEST_LIBRARY}
    ${LIBXSLT_LIBRARIES} ${LIBXML2_LIBRARIES}
    )

foreach(_TEST ${IMPORT_EXPORT_TESTS})
    add_import_export_test(${_TEST})
endforeach(_TEST)
foreach(_TEST ${DATABLOCKS_TESTS})
    add_datablocks_test(${_TEST})
endforeach(_TEST)
foreach(_TEST ${DATABASE_TESTS})
    add_database_test(${_TEST})
endforeach(_TEST)
foreach(_TEST ${UI_TESTS})
    add_ui_test(${_TEST})
endforeach(_TEST)


############# NukeDB #################

if(BUILD_NUKEDB)
   set(nukedb_SRCS
      nukedb/main.cpp
      nukedb/mainwindow.cpp
      nukedb/nukeconfigwidget.cpp
   )

   qt5_wrap_ui(nukedb_SRCS
      nukedb/nukeconfigwidget.ui
   )

   add_executable(nukedb ${nukedb_SRCS})
   target_link_libraries(nukedb krecipescommon KF5::WidgetsAddons)

   install(TARGETS nukedb ${INSTALL_TARGETS_DEFAULT_ARGS})
endif(BUILD_NUKEDB)


############# Main  #################

set(krecipes_SRCS
   main.cpp
   kresplashscreen.cpp
   #krecipes.cpp
   #krecipesview.cpp
   #propertycalculator.cpp
   #shoppingcalculator.cpp
   convert_sqlite3.cpp
   #klomanager.cpp
)

qt5_add_dbus_adaptor(krecipes_SRCS org.kde.krecipes.xml krecipesview.h KrecipesView)

ecm_add_app_icon(krecipes_SRCS ICONS
    16-apps-krecipes.png
    22-apps-krecipes.png
    32-apps-krecipes.png
    48-apps-krecipes.png
    64-apps-krecipes.png
    128-apps-krecipes.png )

add_executable(krecipes
   ${krecipes_SRCS}
   )

target_link_libraries(krecipes krecipescommon ${KDE4_KHTML_LIBS} KF5::WidgetsAddons ${QT_QTSQL_LIBRARY} ${LIBXSLT_LIBRARIES} ${LIBXML2_LIBRARIES} ${QT_QTWEBKIT_LIBRARY} ${KDE4_KDEWEBKIT_LIBS})


install(TARGETS krecipes ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

install( FILES  krecipesui.rc DESTINATION ${DATA_INSTALL_DIR}/krecipes )
install( PROGRAMS  krecipes.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )

ecm_install_icons(ICONS
    16-apps-krecipes.png
    22-apps-krecipes.png
    32-apps-krecipes.png
    48-apps-krecipes.png
    64-apps-krecipes.png
    128-apps-krecipes.png
    DESTINATION ${ICON_INSTALL_DIR}/ )
