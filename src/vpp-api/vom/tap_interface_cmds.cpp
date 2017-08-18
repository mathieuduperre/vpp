/*
 * Copyright (c) 2017 Cisco Systems, Inc. and others.  All rights reserved.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/epl-v10.html
 */

#include <typeinfo>
#include <cassert>
#include <iostream>

#include "vom/cmd.hpp"
#include "vom/tap_interface.hpp"

#include <vapi/tap.api.vapi.hpp>

using namespace VPP;

tap_interface::create_cmd::create_cmd(HW::item<handle_t> &item,
                                       const std::string &name,
                                       route::prefix_t &prefix,
                                       const l2_address_t &l2_address):
    interface::create_cmd<vapi::Tap_connect>(item, name),
    m_prefix(prefix),
    m_l2_address(l2_address)
{
}

rc_t tap_interface::create_cmd::issue(connection &con)
{
    msg_t req(con.ctx(), std::ref(*this));

    auto &payload = req.get_request().get_payload();
    memset(payload.tap_name, 0,
                    sizeof(payload.tap_name));
    memcpy(payload.tap_name, m_name.c_str(),
           std::min(m_name.length(),
                    sizeof(payload.tap_name)));
    if (m_prefix != route::prefix_t::ZERO)
    {
        if (m_prefix.address().is_v6())
        {
            m_prefix.to_vpp(&payload.ip6_address_set,
                            payload.ip6_address,
                            &payload.ip6_mask_width);
        }
        else
        {
            m_prefix.to_vpp(&payload.ip4_address_set,
                            payload.ip4_address,
                            &payload.ip4_mask_width);
            payload.ip4_address_set = 1;
        }
    }

    if (m_l2_address != l2_address_t::ZERO)
    {
        m_l2_address.to_bytes(payload.mac_address, 6);
    }
    else
    {
        payload.use_random_mac = 1;
    }

    VAPI_CALL(req.execute());

    m_hw_item =  wait();

    return rc_t::OK;
}

std::string tap_interface::create_cmd::to_string() const
{
    std::ostringstream s;
    s << "tap-intf-create: " << m_hw_item.to_string()
      << " ip-prefix:" << m_prefix.to_string(); 

    return (s.str());
}

tap_interface::delete_cmd::delete_cmd(HW::item<handle_t> &item):
    interface::delete_cmd<vapi::Tap_delete>(item)
{
}

rc_t tap_interface::delete_cmd::issue(connection &con)
{
    // finally... call VPP

    return rc_t::OK;
}
std::string tap_interface::delete_cmd::to_string() const
{
    std::ostringstream s;
    s << "tap-itf-delete: " << m_hw_item.to_string();

    return (s.str());
}

tap_interface::dump_cmd::dump_cmd()
{
}

bool tap_interface::dump_cmd::operator==(const dump_cmd& other) const
{
    return (true);
}

rc_t tap_interface::dump_cmd::issue(connection &con)
{
    m_dump.reset(new msg_t(con.ctx(), std::ref(*this)));

    VAPI_CALL(m_dump->execute());

    wait();

    return rc_t::INPROGRESS;
}

std::string tap_interface::dump_cmd::to_string() const
{
    return ("tap-itf-dump");
}
