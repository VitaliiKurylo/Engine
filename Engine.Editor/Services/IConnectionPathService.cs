using Engine.Editor.Controls;
using Engine.Editor.Models;
using System.Collections.Generic;
using System.Windows;

namespace Engine.Editor.Services
{
    interface IConnectionPathService
    {
        List<Point> GetConnectionLine(ConnectorInfo source, ConnectorInfo sink, bool showLastLine);
        
        List<Point> GetConnectionLine(ConnectorInfo source, Point sinkPoint, ConnectorOrientation preferredOrientation);
    }
}
