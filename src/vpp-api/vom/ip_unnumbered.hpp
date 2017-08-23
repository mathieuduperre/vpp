/*
 * Copyright (c) 2017 Cisco Systems, Inc. and others.  All rights reserved.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/epl-v10.html
 */

#ifndef __VPP_IP_UNNUMBERED_H__
#define __VPP_IP_UNNUMBERED_H__

#include <string>

#include "vom/object_base.hpp"
#include "vom/om.hpp"
#include "vom/hw.hpp"
#include "vom/rpc_cmd.hpp"
#include "vom/singular_db.hpp"
#include "vom/interface.hpp"
#include "vom/inspect.hpp"

namespace VPP
{
    /**
     * A representation of IP unnumbered configuration on an interface
     */
    class ip_unnumbered: public object_base
    {
    public:
        /**
         * Construct a new object matching the desried state
         *
         * @param itf - The interface with no IP address
         * @param l3_itf - The interface that has the IP address we wish to share.
         */
        ip_unnumbered(const interface &itf,
                      const interface &l3_itf);
        
        /**
         * Copy Constructor
         */
        ip_unnumbered(const ip_unnumbered& o);

        /**
         * Destructor
         */
        ~ip_unnumbered();

        /**
         * Return the 'singular instance' of the L3-Config that matches this object
         */
        std::shared_ptr<ip_unnumbered> singular() const;

        /**
         * convert to string format for debug purposes
         */
        std::string to_string() const;

        /**
         * Dump all ip_unnumbereds into the stream provided
         */
        static void dump(std::ostream &os);

        /**
         * The key type for ip_unnumbereds
         */
        typedef interface::key_type key_t;

        /**
         * Find an singular instance in the DB for the interface passed
         */
        static std::shared_ptr<ip_unnumbered> find(const interface &i);

        /**
         * A command class that configures the IP unnumbered
         */
        class config_cmd: public rpc_cmd<HW::item<bool>, rc_t,
                                         vapi::Sw_interface_set_unnumbered>
        {
        public:
            /**
             * Constructor
             */
            config_cmd(HW::item<bool> &item,
                       const handle_t &itf,
                       const handle_t &l3_itf);

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
            bool operator==(const config_cmd&i) const;
        private:
            /**
             * Reference to the interface for which the address is required
             */
            const handle_t &m_itf;
            /**
             * Reference to the interface which has an address
             */
            const handle_t &m_l3_itf;
        };

        /**
         * A cmd class that Unconfigs L3 Config from an interface
         */
        class unconfig_cmd: public rpc_cmd<HW::item<bool>, rc_t,
                                           vapi::Sw_interface_set_unnumbered>
        {
        public:
            /**
             * Constructor
             */
            unconfig_cmd(HW::item<bool> &item,
                         const handle_t &itf,
                         const handle_t &l3_itf);

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
            bool operator==(const unconfig_cmd&i) const;
        private:
            /**
             * Reference to the interface for which the address is required
             */
            const handle_t &m_itf;
            /**
             * Reference to the interface which has an address
             */
            const handle_t &m_l3_itf;
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
        void update(const ip_unnumbered &obj);

        /**
         * Find or add the singular instance in the DB
         */
        static std::shared_ptr<ip_unnumbered> find_or_add(const ip_unnumbered &temp);

        /*
         * It's the VPPHW class that updates the objects in HW
         */
        friend class VPP::OM;

        /**
           e* It's the VPP::singular_db class that calls replay()
        */
        friend class VPP::singular_db<key_t, ip_unnumbered>;

        /**
         * Sweep/reap the object if still stale
         */
        void sweep(void);

        /**
         * replay the object to create it in hardware
         */
        void replay(void);

        /**
         * A reference counting pointer the interface that requires an address.
         */
        const std::shared_ptr<interface> m_itf;
        /**
         * A reference counting pointer the interface that has an address.
         */
        const std::shared_ptr<interface> m_l3_itf;

        /**
         * HW configuration for the binding. The bool representing the
         * do/don't bind.
         */
        HW::item<bool> m_config;

        /**
         * A map of all L3 configs keyed against a combination of the interface
         * and subnet's keys.
         */
        static singular_db<key_t, ip_unnumbered> m_db;
    };
};

#endif
