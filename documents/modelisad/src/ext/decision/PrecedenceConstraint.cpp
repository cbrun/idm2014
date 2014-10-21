/*
 * @file ext/decision/PrecedenceConstraint.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <ext/decision/PrecedenceConstraint.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>

namespace modelisad { namespace extension { namespace decision {

PrecedenceConstraint::PrecedenceConstraint(iterator first,
                                           iterator second,
                                           Type type,
                                           const vle::devs::Time& mintimelag,
                                           const vle::devs::Time& maxtimelag)
    : m_type(type), m_mintimelag(mintimelag), m_maxtimelag(maxtimelag),
    m_first(first), m_second(second)
{
    if (mintimelag < 0.0) {
        throw vle::utils::ModellingError(
            _("Decision: Precedence constraint, bad min TL"));
    }

    if (mintimelag > maxtimelag) {
        throw vle::utils::ModellingError(
            _("Decision: Precedence constraint, FS min >= max TL"));
    }
}

PrecedenceConstraint::Result
PrecedenceConstraint::isValid(const vle::devs::Time& time) const
{
    const Activity& i(first()->second);
    const Activity& j(second()->second);

    switch (m_type) {
    case SS:
        switch (i.state()) {
        case Activity::WAIT:
            switch (j.state()) {
            case Activity::WAIT:
                return std::make_pair(Wait, vle::devs::Time::infinity);
            case Activity::STARTED:
            case Activity::FF:
            case Activity::DONE:
                throw vle::utils::InternalError(_("Decision: impossible state (SS)"));
            case Activity::FAILED:
                return std::make_pair(Failed, vle::devs::Time::infinity);
            }
        case Activity::STARTED:
            switch (j.state()) {
            case Activity::WAIT:
                {
                    vle::devs::Time dmin = i.startedDate() + mintimelag();
                    vle::devs::Time dmax = i.startedDate() + maxtimelag();

                    if (i.startedDate() <= time and time == dmin and
                        time == dmax) {
                        return std::make_pair(Valid, dmin);
                    } else if (i.startedDate() <= time and time < dmin) {
                        return std::make_pair(Wait, dmin);
                    } else if (dmin <= time and time <= dmax) {
                        return std::make_pair(Valid, dmax);
                    } else if (dmax < time) {
                        return std::make_pair(Failed, vle::devs::Time::infinity);
                    }
                }
            case Activity::STARTED:
            case Activity::FF:
                return std::make_pair(Valid, vle::devs::Time::infinity);
            case Activity::DONE:
                return std::make_pair(Inapplicable, vle::devs::Time::infinity);
            case Activity::FAILED:
                return std::make_pair(Failed, vle::devs::Time::infinity);
            }
        case Activity::FF:
            switch (j.state()) {
            case Activity::WAIT:
                {
                    vle::devs::Time dmin = i.startedDate() + mintimelag();
                    vle::devs::Time dmax = i.startedDate() + maxtimelag();

                    if (i.startedDate() <= time and time == dmin and
                        time == dmax) {
                        return std::make_pair(Valid, dmin);
                    } else if (i.startedDate() <= time and time < dmin) {
                        return std::make_pair(Wait, dmin);
                    } else if (dmin <= time and time <= dmax) {
                        return std::make_pair(Valid, dmax);
                    } else if (dmax < time) {
                        return std::make_pair(Failed, vle::devs::Time::infinity);
                    }
                }
            case Activity::STARTED:
            case Activity::FF:
                return std::make_pair(Valid, vle::devs::Time::infinity);
            case Activity::DONE:
                return std::make_pair(Inapplicable, vle::devs::Time::infinity);
            case Activity::FAILED:
                return std::make_pair(Failed, vle::devs::Time::infinity);
            }
        case Activity::DONE:
            switch (j.state()) {
            case Activity::WAIT:
                {
                    vle::devs::Time dmin = i.startedDate() + mintimelag();
                    vle::devs::Time dmax = i.startedDate() + maxtimelag();
                    if((dmin <= time) && (time <= dmax)){
                        return std::make_pair(Valid, vle::devs::Time::infinity);
                    } else if (time < dmin){
                        return std::make_pair(Wait, dmin);
                    } else if (dmax < time){
                        return std::make_pair(Failed, vle::devs::Time::infinity);
                    }
                }
            case Activity::STARTED:
            case Activity::FF:
            case Activity::DONE:
                return std::make_pair(Inapplicable, vle::devs::Time::infinity);
            case Activity::FAILED:
                return std::make_pair(Failed, vle::devs::Time::infinity);
            }
        case Activity::FAILED:
            switch (j.state()) {
            case Activity::WAIT:
                return std::make_pair(Failed, vle::devs::Time::infinity);
            case Activity::STARTED:
            case Activity::FF:
            case Activity::DONE:
                //i must have failed after start.
                return std::make_pair(Inapplicable, vle::devs::Time::infinity);
            case Activity::FAILED:
                return std::make_pair(Failed, vle::devs::Time::infinity);
            }
        }
        break;

    case FF:
        switch (i.state()) {
        case Activity::WAIT:
            switch (j.state()) {
            case Activity::WAIT:
            case Activity::STARTED:
            case Activity::FF:
            case Activity::DONE:
                return std::make_pair(Inapplicable, vle::devs::Time::infinity);
            case Activity::FAILED:
                return std::make_pair(Failed, vle::devs::Time::infinity);
            }
        case Activity::STARTED:
            switch (j.state()) {
            case Activity::WAIT:
                return std::make_pair(Inapplicable, vle::devs::Time::infinity);
            case Activity::STARTED:
                return std::make_pair(Inapplicable, vle::devs::Time::infinity);
            case Activity::FF:
                return std::make_pair(Failed, vle::devs::Time::infinity);
            case Activity::DONE:
                return std::make_pair(Inapplicable, vle::devs::Time::infinity);
            case Activity::FAILED:
                return std::make_pair(Failed, vle::devs::Time::infinity);
            }
        case Activity::FF:
            switch (j.state()) {
            case Activity::WAIT:
            case Activity::STARTED:
            case Activity::FF:
                {
                    vle::devs::Time dmin = i.ffDate() + mintimelag();
                    vle::devs::Time dmax = i.ffDate() + maxtimelag();

                    if (i.ffDate() <= time and time == dmin and
                        time == dmax) {
                        return std::make_pair(Valid, dmin);
                    } else if (i.ffDate() <= time and time < dmin) {
                        return std::make_pair(Wait, dmin);
                    } else if (dmin <= time and time <= dmax) {
                        return std::make_pair(Valid, dmax);
                    } else if (dmax < time) {
                        return std::make_pair(Failed, vle::devs::Time::infinity);
                    }
                }
            case Activity::DONE:
                return std::make_pair(Valid, vle::devs::Time::infinity);
            case Activity::FAILED:
                return std::make_pair(Failed, vle::devs::Time::infinity);
            }
        case Activity::DONE:
            switch (j.state()) {
            case Activity::WAIT:
            case Activity::STARTED:
            case Activity::FF:
            case Activity::DONE:
                {
                    vle::devs::Time dmin = i.ffDate() + mintimelag();
                    vle::devs::Time dmax = i.ffDate() + maxtimelag();

                    if (i.ffDate() <= time and time == dmin and
                        time == dmax) {
                        return std::make_pair(Valid, dmin);
                    } else if (i.ffDate() <= time and time < dmin) {
                        return std::make_pair(Wait, dmin);
                    } else if (dmin <= time and time <= dmax) {
                        return std::make_pair(Valid, dmax);
                    } else if (dmax < time) {
                        return std::make_pair(Failed, vle::devs::Time::infinity);
                    }
                }
            case Activity::FAILED:
                return std::make_pair(Failed, vle::devs::Time::infinity);
            }
        case Activity::FAILED:
            return std::make_pair(Failed, vle::devs::Time::infinity);
        }
        break;

    case FS:
        switch (i.state()) {
        case Activity::WAIT:
            switch (j.state()) {
            case Activity::WAIT:
                return std::make_pair(Wait, vle::devs::Time::infinity);
            case Activity::STARTED:
            case Activity::FF:
            case Activity::DONE:
                return std::make_pair(Inapplicable, vle::devs::Time::infinity);
            case Activity::FAILED:
                return std::make_pair(Failed, vle::devs::Time::infinity);
            }
        case Activity::STARTED:
            switch (j.state()) {
            case Activity::WAIT:
                return std::make_pair(Wait, vle::devs::Time::infinity);
            case Activity::STARTED:
            case Activity::FF:
            case Activity::DONE:
                return std::make_pair(Inapplicable, vle::devs::Time::infinity);
            case Activity::FAILED:
                return std::make_pair(Failed, vle::devs::Time::infinity);
            }
        case Activity::FF:
            switch (j.state()) {
            case Activity::WAIT:
                return std::make_pair(Wait, vle::devs::Time::infinity);
            case Activity::STARTED:
            case Activity::FF:
            case Activity::DONE:
                return std::make_pair(Inapplicable, vle::devs::Time::infinity);
            case Activity::FAILED:
                return std::make_pair(Failed, vle::devs::Time::infinity);
            }
        case Activity::DONE:
            switch (j.state()) {
            case Activity::WAIT:
                {
                    vle::devs::Time dmin = i.doneDate() + mintimelag();
                    vle::devs::Time dmax = i.doneDate() + maxtimelag();

                    if (i.doneDate() <= time and time == dmin and
                        time == dmax) {
                        return std::make_pair(Valid, vle::devs::Time::infinity);
                    } else if (i.doneDate() <= time and time < dmin) {
                        return std::make_pair(Wait, dmin);
                    } else if (dmin <= time and time <= dmax) {
                        return std::make_pair(Valid, vle::devs::Time::infinity);
                    } else if (dmax < time) {
                        return std::make_pair(Failed, vle::devs::Time::infinity);
                    }
                }
            case Activity::STARTED:
            case Activity::FF:
            case Activity::DONE:
                return std::make_pair(Inapplicable, vle::devs::Time::infinity);
            case Activity::FAILED:
                return std::make_pair(Failed, vle::devs::Time::infinity);
            }
        case Activity::FAILED:
            return std::make_pair(Failed, vle::devs::Time::infinity);
        }
        break;
    }

    throw vle::utils::InternalError(vle::fmt(
            _("Decision: unknown precedence constraint %1%")) % (int)m_type);
}

}}} // namespace modelisad model decision

