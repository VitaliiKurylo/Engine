using Engine.Editor.Controls;
using PropertyChanged;
using System.ComponentModel;

namespace Engine.Editor.Models
{
    [ImplementPropertyChanged()]
    class FullyCreatedConnectorModel : AbstractConnectorModel
    {
        public FullyCreatedConnectorModel(AbstractModel targetModel, ConnectorOrientation orientation)
            : base(orientation)
        {
            Target = targetModel;
        }
        
        [Browsable(false)]
        public AbstractModel Target { get; }
        
        public bool ShowConnectors { get; set; }
    }
}
