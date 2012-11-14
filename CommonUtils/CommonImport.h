#ifndef __COMMON_IMPORT_H
#define __COMMON_IMPORT_H

#include <string>
#include <vector>

#include "CommonSceneGraph.h"
#include "CommonAlembic.h"
#include "CommonAbcCache.h"

class IJobStringParser
{
public:

   bool importNormals;
   bool importUVs;
   bool importMaterialIds;
   bool importFacesets;
   bool importStandinProperties;
   bool importBoundingBoxes;//import boundng boxes instead of shape
   bool attachToExisting;
   bool failOnUnsupported;
   bool importVisibilityControllers;

	std::string filename;// = EC_MCHAR_to_UTF8( strPath );

   std::vector<std::string> nodesToImport;

   bool includeChildren;

   IJobStringParser():
      importNormals(false),
      importUVs(true),
      importMaterialIds(false),
      importFacesets(true),
      importStandinProperties(false),
      importBoundingBoxes(false),
      attachToExisting(false),
      failOnUnsupported(false),
      importVisibilityControllers(false),
      includeChildren(false)
   {}

   bool parse(const std::string& jobString);

   std::string buildJobString();
};





class SceneNodeAlembic;
class SceneNodeApp : public SceneNode
{
public:

   virtual bool replaceData(SceneNodePtr fileNode, const IJobStringParser& jobParams){ return false; }
   virtual bool addChild(SceneNodePtr fileNode, const IJobStringParser& jobParams, SceneNodePtr newAppNode){ return false; }

};

class SceneNodeAlembic : public SceneNode
{
public:
   Abc::IObject iObj;
   bool bWasMerged;

   SceneNodeAlembic(Abc::IObject& obj):iObj(obj), bWasMerged(false)
   {}

   virtual Abc::IObject getObject();

   virtual bool wasMerged();
   virtual void setMerged(bool bMerged=true);
};




SceneNodePtr buildCommonSceneGraph(AbcArchiveCache *pArchiveCache, AbcObjectCache *pRootObjectCache, int& nNumNodes);



bool ImportSceneFile(const IJobStringParser& jobParams, SceneNodePtr fileRoot, SceneNodePtr appRoot);
bool AttachSceneFile(const IJobStringParser& jobParams, SceneNodePtr fileRoot, SceneNodePtr appRoot);



#endif