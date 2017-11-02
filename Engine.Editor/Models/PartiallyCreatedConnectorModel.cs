using Engine.Editor.Controls;
using System.Windows;

namespace Engine.Editor.Models
{
    class PartiallyCreatedConnectorModel : AbstractConnectorModel
    {
        public PartiallyCreatedConnectorModel(Point currentLocation)
            : base(ConnectorOrientation.None)
        {
            CurrentLocation = currentLocation;
        }

        public Point CurrentLocation { get; }
    }
}
