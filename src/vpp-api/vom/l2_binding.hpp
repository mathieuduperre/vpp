/*
 * Copyright (c) 2017 Cisco Systems, Inc. and others.  All rights reserved.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/epl-v10.html
 */

#ifndef __VPP_L2_INTERFACE_H__
#define __VPP_L2_INTERFACE_H__

#include <string>
#include <map>
#include <stdint.h>

#include "vom/object_base.hpp"
#include "vom/om.hpp"
#include "vom/hw.hpp"
#include "vom/rpc_cmd.hpp"
#include "vom/singular_db.hpp"
#include "vom/interface.hpp"
#include "vom/bridge_domain.hpp"
#include "vom/vxlan_tunnel.hpp"
#include "vom/inspect.hpp"

namespace VPP
{
    /**
     * A base class for all object_base in the VPP object_base-Model.
     *  provides the abstract interface.
     */
    class l2_binding: public object_base
    {
    public:
        /**
         * Construct a new object matching the desried state
         */
        l2_binding(const interface &itf,
                   const bridge_domain &bd);

        /**
         * Copy Constructor
         */
        l2_binding(const l2_binding& o);

        /**
         * Destructor
         */
        ~l2_binding();

        /**
         * Return the 'singular instance' of the L2 config that matches this object
         */
        std::shared_ptr<l2_binding> singular() const;

        /**
         * convert to string format for debug purposes
         */
        std::string to_string() const;

        /**
         * Dump all l2_bindings into the stream provided
         */
        static void dump(std::ostream &os);

        /**
         * A functor class that binds L2 configuration to an interface
         */
        class bind_cmd: public rpc_cmd<HW::item<bool>, rc_t,
                                       vapi::Sw_interface_set_l2_bridge>
        {
        public:
            /**
             * Constructor
             */
            bind_cmd(HW::item<bool> &item,
                     const handle_t &itf,
                     uint32_t bd,
                     bool is_bvi);

            /**
             * Issue the command to VPP/HW
             */
            rc_t issue(connection &con);
            /**
             * convert to string format for debug purposes
             */
            std::string to_string() const;

            /**
             * Comparison operator - only used for UT
             */
            bool operator==(const bind_cmd&i) const;
        private:
            /**
             * The interface to bind
             */
            const handle_t m_itf;

            /**
             * The bridge-domain to bind to
             */
            uint32_t m_bd;

            /**
             * Is it a BVI interface that is being bound
             */
            bool m_is_bvi;
        };

        /**
         * A cmd class that Unbinds L2 configuration from an interface
         */
        class unbind_cmd: public rpc_cmd<HW::item<bool>, rc_t,
                                         vapi::Sw_interface_set_l2_bridge>
        {
        public:
            /**
             * Constructor
             */
            unbind_cmd(HW::item<bool> &item,
                       const handle_t &itf,
                       uint32_t bd,
                       bool is_bvi);

            /**
             * Issue the command to VPP/HW
             */
            rc_t issue(connection &con);
            /**
             * convert to string format for debug purposes
             */
            std::string to_string() const;

            /**
             * Comparison operator - only used for UT
             */
            bool operator==(const unbind_cmd&i) const;
        private:
            /**
             * The interface to bind
             */
            const handle_t m_itf;

            /**
             * The bridge-domain to bind to
             */
            uint32_t m_bd;

            /**
             * Is it a BVI interface that is being bound
             */
            bool m_is_bvi;
        };

    private:
        /**
         * Class definition for listeners to OM events
         */
        class event_handler: public OM::listener, public inspect::command_handler
        {
        public:
            event_handler();
            virtual ~event_handler() = default;

            /**
             * Handle a populate event
             */
            void handle_populate(const client_db::key_t & key);

            /**
             * Handle a replay event
             */
            void handle_replay();

            /**
             * Show the object in the Singular DB
             */
            void show(std::ostream &os);

            /**
             * Get the sortable Id of the listener
             */
            dependency_t order() const;
        };

        /**
         * event_handler to register with OM
         */
        static event_handler m_evh;

        /**
         * Enquue commonds to the VPP command Q for the update
         */
        void update(const l2_binding &obj);

        /**
         * Find or Add the singular instance in the DB
         */
        static std::shared_ptr<l2_binding> find_or_add(const l2_binding &temp);

        /*
         * It's the VPP::OM class that calls singular()
         */
        friend class VPP::OM;

        /**
         * It's the VPP::singular_db class that calls replay()
         */
        friend class VPP::singular_db<const handle_t, l2_binding>;

        /**
         * Sweep/reap the object if still stale
         */
        void sweep(void);

        /**
         * replay the object to create it in hardware
         */
        void replay(void);

        /**
         * A reference counting pointer the interface that this L2 layer
         * represents. By holding the reference here, we can guarantee that
         * this object will outlive the interface
         */
        const std::shared_ptr<interface> m_itf;
    
        /**
         * A reference counting pointer the Bridge-Domain that this L2
         * interface is bound to. By holding the reference here, we can
         * guarantee that this object will outlive the BD.
         */
        const std::shared_ptr<bridge_domain> m_bd;

        /**
         * HW configuration for the binding. The bool representing the
         * do/don't bind.
         */
        HW::item<bool> m_binding;

        /**
         * A map of all L2 interfaces key against the interface's handle_t
         */
        static singular_db<const handle_t, l2_binding> m_db;
    };
};

#endif
