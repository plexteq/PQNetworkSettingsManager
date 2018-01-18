using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NetworkSettingsAdapter
{
    public class AdapterEntity
    {
        public string UUID { get; set; }
        public string IpAddress { get; set; }
        public bool IsDhcpEnabled { get; set; }
        public string NetMask { get; set; }
        public string DefaultIpGateway { get; set; }
        public string[] DNS { get; set; }
    }
}
