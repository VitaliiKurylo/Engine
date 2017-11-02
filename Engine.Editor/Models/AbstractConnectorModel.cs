using Engine.Editor.Controls;
using Engine.Editor.Operations;

namespace Engine.Editor.Models
{
    abstract class AbstractConnectorModel : OperationObject
    {
        protected AbstractConnectorModel(ConnectorOrientation orientation)
        {
            Orientation = orientation;
        }
        
        public ConnectorOrientation Orientation { get; }
    }
}
