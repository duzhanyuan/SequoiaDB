using SequoiaDB;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using SequoiaDB.Bson;
using System.Collections.Generic;

namespace DriverTest
{
    [TestClass()]
    public class ReplicaGroupTest
    {
        private TestContext testContextInstance;
        private static Config config = null;
        private static Sequoiadb sdb = null;
        private static ReplicaGroup group = null;
        private static SequoiaDB.Node node = null;
        private static SequoiaDB.Node node2 = null;

        private static string groupName = null;
        private static string hostName = null;
        private static int port = -1;
        private static string dbpath = null;

        private static string groupName2 = null;
        private static string hostName2 = null;
        private static int port2 = -1;
        private static string dbpath2 = null;

        private static bool connect_flag = false;
        private static bool create_rg_flag = false;
        private static bool create_node_flag = false;
        private static bool create_node2_flag = false;

        public TestContext TestContext
        {
            get
            {
                return testContextInstance;
            }
            set
            {
                testContextInstance = value;
            }
        }

        #region 附加测试特性
        //使用 SequoiadbInitialize 在运行类中的第一个测试前先运行代码
        [ClassInitialize()]
        public static void SequoiadbInitialize(TestContext testContext)
        {
            if ( config == null )
                config = new Config();
            sdb = new Sequoiadb(config.conf.Coord.Address);
            sdb.Connect(config.conf.UserName, config.conf.Password);
            // check whether it is in the cluster environment or not
            if (!Constants.isClusterEnv(sdb))
            {
                return;
            }
            // argument
            groupName = config.conf.Groups[3].GroupName;
            hostName = config.conf.Groups[3].Nodes[0].HostName;
            port = config.conf.Groups[3].Nodes[0].Port;
            dbpath = config.conf.Groups[3].Nodes[0].DBPath;
            // drop the exist group
            group = sdb.GetReplicaGroup(groupName);
            if (group != null)
            {
                // drop all the cs in current group, and then remove this group
                int nodeNum = group.GetNodeNum(SDBConst.NodeStatus.SDB_NODE_ALL);
                if (nodeNum > 0)
                {
                    SequoiaDB.Node nd = group.GetMaster();
                    Sequoiadb db = new Sequoiadb(nd.HostName, nd.Port);
                    Assert.IsTrue(nd.Start());
                    db.Connect(config.conf.UserName, config.conf.Password);
                    DBCursor cursor = db.ListCollectionSpaces();
                    while (cursor.Next() != null)
                    {
                        BsonDocument obj = cursor.Current();
                        string temp = null;
                        if (obj.Contains("Name"))
                            temp = obj["Name"].AsString;
                        sdb.DropCollectionSpace(temp);
                    }
                    db.Disconnect();
                }
                try
                {
                    sdb.RemoveReplicaGroup(group.GroupName);
                }
                catch (BaseException e)
                {
                    string errInfo = e.Message;
                    Console.WriteLine("Error code is: " + errInfo);
                }
            }
            // create a new group
            group = sdb.CreateReplicaGroup(groupName);
            Assert.IsTrue(groupName.Equals(group.GroupName));
            create_rg_flag = true;
            // create a node
            BsonDocument options = new BsonDocument();
            options.Add("logfilenum", 1);
            node = group.CreateNode(hostName, port, dbpath, options);
            Assert.IsNotNull(node);
            node.Start();
            create_node_flag = true;
        }
        //使用 SequoiadbCleamUp 在运行完类中的所有测试后再运行代码
        [ClassCleanup()]
        public static void SequoiadbCleamUp()
        {
            // check whether it is in the cluster environment or not
            if (!Constants.isClusterEnv(sdb))
            {
                return;
            }
            group = sdb.GetReplicaGroup(groupName);
            if (null != group)
            {
                // drop all the cs in current group, and then remove this group
                int nodeNum = group.GetNodeNum(SDBConst.NodeStatus.SDB_NODE_ALL);
                if (nodeNum > 0)
                {
                    SequoiaDB.Node nd = group.GetMaster();
                    Sequoiadb db = new Sequoiadb(nd.HostName, nd.Port);
                    Assert.IsTrue(nd.Start());
                    db.Connect(config.conf.UserName, config.conf.Password);
                    DBCursor cursor = db.ListCollectionSpaces();
                    while (cursor.Next() != null)
                    {
                        BsonDocument obj = cursor.Current();
                        string temp = null;
                        if (obj.Contains("Name"))
                            temp = obj["Name"].AsString;
                        sdb.DropCollectionSpace(temp);
                    }
                    db.Disconnect();
                }
                Assert.IsTrue(group.Stop());
                // remove group
                try
                {
                    sdb.RemoveReplicaGroup(group.GroupName);
                }
                catch (BaseException e)
                {
                    string errInfo = e.Message;
                    Console.WriteLine("Error code is: " + errInfo);
                }
            }
            create_node_flag = false;
            create_node2_flag = false;
            // disconnect
            sdb.Disconnect();
        }
        //使用 TestInitialize 在运行每个测试前先运行代码
        [TestInitialize()]
        public void MyTestInitialize()
        {
            // check whether it is in the cluster environment or not
            if (!Constants.isClusterEnv(sdb))
            {
                return;
            }
            // init
            create_node2_flag = false;
            // argument
            groupName2 = config.conf.Groups[3].GroupName;
            hostName2 = config.conf.Groups[3].Nodes[1].HostName;
            port2 = config.conf.Groups[3].Nodes[1].Port;
            dbpath2 = config.conf.Groups[3].Nodes[1].DBPath;
            // drop the exist group
            group = sdb.GetReplicaGroup(groupName);
            if (null == group)
                return;
            // create a node
            BsonDocument options = new BsonDocument();
            options.Add("logfilenum", 1);
            node2 = group.CreateNode(hostName2, port2, dbpath2, options);
            Assert.IsNotNull(node2);
            node2.Start();
        }
        //使用 TestCleanup 在运行完每个测试后运行代码
        [TestCleanup()]
        public void MyTestCleanup()
        {
            // check whether it is in the cluster environment or not
            if (false == Constants.isClusterEnv(sdb))
            {
                return;
            }
            group = sdb.GetReplicaGroup(groupName);
            if (null != group)
            {
                group.RemoveNode(hostName2, port2, null);
            }
            create_node2_flag = false;
        }
        #endregion

        [TestMethod()]
        [Ignore]
        public void RGTest()
        {
            // check whether it is in the cluster environment or not
            if (!Constants.isClusterEnv(sdb))
            {
                Console.WriteLine("It's not a cluster environment.");
                return;
            }
            group = sdb.GetReplicaGroup(groupName);
            if (group == null)
                group = sdb.CreateReplicaGroup(groupName);
            ReplicaGroup group1 = sdb.GetReplicaGroup(group.GroupID);
            Assert.AreEqual(group.GroupName, group1.GroupName);
            ReplicaGroup group2 = sdb.GetReplicaGroup(1);
            Assert.IsNotNull(group2);
            node = group.GetNode(hostName, port);
            if (node == null)
            {
                string dbpath = config.conf.Groups[0].Nodes[0].DBPath;
                Dictionary<string, string> map = new Dictionary<string, string>();
                map.Add("diaglevel", config.conf.Groups[0].Nodes[0].DiagLevel);
                node = group.CreateNode(hostName, port, dbpath, map);
            }
            group.Start();
            int num = group.GetNodeNum(SDBConst.NodeStatus.SDB_NODE_ALL);
            Assert.IsTrue(num > 0);
            BsonDocument detail = group.GetDetail();
            string gn = detail["GroupName"].AsString;
            Assert.IsTrue(groupName.Equals(gn));
            SequoiaDB.Node master = group.GetMaster();
            Assert.IsNotNull(master);
            SequoiaDB.Node slave = group.GetSlave();
            Assert.IsNotNull(slave);
            Assert.IsTrue(node.Stop());
            Assert.IsTrue(node.Start());
            SDBConst.NodeStatus status = node.GetStatus();
            Assert.IsTrue(status == SDBConst.NodeStatus.SDB_NODE_ACTIVE);

            Sequoiadb db = node.Connect(config.conf.UserName, config.conf.Password);
            db.Disconnect();
            node.Stop();
            group.Stop();
        }

        [TestMethod()]
        [Ignore]
        public void removeRG()
        {
            // check whether it is in the cluster environment or not
            if (!Constants.isClusterEnv(sdb))
            {
                Console.WriteLine("It's not a cluster environment.");
                return;
            }
            // get rg
            group = sdb.GetReplicaGroup(groupName);
            if (group == null)
                group = sdb.CreateReplicaGroup(groupName);
            Assert.IsNotNull(group);
            // create node1
            string hostName1 = config.conf.Groups[1].Nodes[1].HostName;
            int port1 = config.conf.Groups[1].Nodes[1].Port;
            string dbPath1 = config.conf.Groups[1].Nodes[1].DBPath;
            Dictionary<string, string> map1 = new Dictionary<string, string>();
            map1.Add("diaglevel", config.conf.Groups[1].Nodes[1].DiagLevel);
            SequoiaDB.Node node1 = group.CreateNode(hostName1, port1, dbPath1, map1);
            Assert.IsNotNull(node1);
            // start node1
            Assert.IsTrue(node1.Start());

            // remove the newly build node
            try
            {
                group.RemoveNode(hostName1, port1, null);
            }
            catch (BsonException e)
            {
                string errInfo = e.Message;
                Console.WriteLine("Error code is: " + errInfo);
            }
            group.Stop();
        }

        [TestMethod()]
        public void attach_and_detach_node()
        {
            SequoiaDB.Node data_node = null;

            // check whether it is in the cluster environment or not
            if (!Constants.isClusterEnv(sdb))
            {
                Console.WriteLine("It's not a cluster environment.");
                return;
            }
            // detach node
            group.DetachNode(hostName2, port2, null);

            // check
            data_node = group.GetNode(hostName2, port2);
            Assert.IsNull(data_node);

            //attach node 
            group.AttachNode(hostName2, port2, null);

            // check
            data_node = group.GetNode(hostName2, port2);
            Assert.IsNotNull(data_node);
        }

    }
}
