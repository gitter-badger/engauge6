#ifndef CMD_ADD_POINT_GRAPH_H
#define CMD_ADD_POINT_GRAPH_H

#include "CmdAbstract.h"
#include <QPointF>

class QXmlStreamReader;
class Transformation;

/// Command for adding one graph point.
class CmdAddPointGraph : public CmdAbstract
{
 public:
  /// Constructor for normal creation
  CmdAddPointGraph(MainWindow &mainWindow,
                   Document &document,
                   const QString &curveName,
                   const QPointF &posScreen,
                   double ordinal);

  /// Constructor for parsing error report file xml
  CmdAddPointGraph(MainWindow &mainWindow,
                   Document &document,
                   const QString &cmdDescription,
                   QXmlStreamReader &reader);

  virtual ~CmdAddPointGraph();

  virtual void cmdRedo ();
  virtual void cmdUndo ();
  virtual void saveXml (QXmlStreamWriter &writer) const;

private:
  CmdAddPointGraph();

  QString m_curveName;

  QPointF m_posScreen;
  double m_ordinal;

  QString m_identifierAdded; // Point that got added. This is generated by the Document during the Redo
};

#endif // CMD_ADD_POINT_GRAPH_H
