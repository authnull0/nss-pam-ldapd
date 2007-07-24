/*
   ldap-schema.c - LDAP schema information functions and definitions
   This file was part of the nss_ldap library which has been
   forked into the nss-ldapd library.

   Copyright (C) 1997-2005 Luke Howard
   Copyright (C) 2006 West Consulting
   Copyright (C) 2006 Arthur de Jong

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301 USA
*/

#include "config.h"

#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#ifdef HAVE_LBER_H
#include <lber.h>
#endif
#ifdef HAVE_LDAP_H
#include <ldap.h>
#endif
#if defined(HAVE_THREAD_H)
#include <thread.h>
#elif defined(HAVE_PTHREAD_H)
#include <pthread.h>
#endif

#include "ldap-nss.h"
#include "ldap-schema.h"
#include "util.h"
#include "attmap.h"

/* max number of attributes per object class */
#define ATTRTAB_SIZE    15

/**
 * declare filters formerly declared in ldap-*.h
 */

/* rfc822 mail aliases */
char _nss_ldap_filt_getaliasbyname[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_getaliasent[LDAP_FILT_MAXSIZ];

/* MAC address mappings */
char _nss_ldap_filt_gethostton[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_getntohost[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_getetherent[LDAP_FILT_MAXSIZ];

/* groups */
char _nss_ldap_filt_getgrnam[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_getgrgid[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_getgrent[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_getgroupsbymemberanddn[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_getgroupsbydn[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_getpwnam_groupsbymember[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_getgroupsbymember[LDAP_FILT_MAXSIZ];

/* IP hosts */
char _nss_ldap_filt_gethostbyname[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_gethostbyaddr[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_gethostent[LDAP_FILT_MAXSIZ];

/* IP networks */
char _nss_ldap_filt_getnetbyname[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_getnetbyaddr[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_getnetent[LDAP_FILT_MAXSIZ];

/* IP protocols */
char _nss_ldap_filt_getprotobyname[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_getprotobynumber[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_getprotoent[LDAP_FILT_MAXSIZ];

/* users */
char _nss_ldap_filt_getpwnam[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_getpwuid[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_getpwent[LDAP_FILT_MAXSIZ];

/* RPCs */
char _nss_ldap_filt_getrpcbyname[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_getrpcbynumber[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_getrpcent[LDAP_FILT_MAXSIZ];

/* IP services */
char _nss_ldap_filt_getservbyname[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_getservbynameproto[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_getservbyport[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_getservbyportproto[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_getservent[LDAP_FILT_MAXSIZ];

/* shadow users */
char _nss_ldap_filt_getspnam[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_getspent[LDAP_FILT_MAXSIZ];

/* netgroups */
char _nss_ldap_filt_getnetgrent[LDAP_FILT_MAXSIZ];
char _nss_ldap_filt_innetgr[LDAP_FILT_MAXSIZ];

/**
 * lookup filter initialization
 */
void
_nss_ldap_init_filters ()
{
  /* rfc822 mail aliases */
  snprintf (_nss_ldap_filt_getaliasbyname, LDAP_FILT_MAXSIZ,
            "(&(%s=%s)(%s=%s))", attmap_objectClass, attmap_alias_objectClass,
            attmap_alias_cn, "%s");
  snprintf (_nss_ldap_filt_getaliasent, LDAP_FILT_MAXSIZ,
            "(%s=%s)", attmap_objectClass, attmap_alias_objectClass);

  /* MAC address mappings */
  snprintf (_nss_ldap_filt_gethostton, LDAP_FILT_MAXSIZ,
            "(&(%s=%s)(%s=%s))", attmap_objectClass, attmap_ether_objectClass,
            attmap_ether_cn, "%s");
  snprintf (_nss_ldap_filt_getntohost, LDAP_FILT_MAXSIZ,
            "(&(%s=%s)(%s=%s))", attmap_objectClass, attmap_ether_objectClass, attmap_ether_macAddress,
            "%s");
  snprintf (_nss_ldap_filt_getetherent, LDAP_FILT_MAXSIZ, "(%s=%s)",
            attmap_objectClass, attmap_ether_objectClass);

  /* groups */
  snprintf (_nss_ldap_filt_getgrnam, LDAP_FILT_MAXSIZ,
            "(&(%s=%s)(%s=%s))", attmap_objectClass, attmap_group_objectClass,
            attmap_group_cn, "%s");
  snprintf (_nss_ldap_filt_getgrgid, LDAP_FILT_MAXSIZ,
            "(&(%s=%s)(%s=%s))", attmap_objectClass, attmap_group_objectClass,
            attmap_group_gidNumber, "%d");
  snprintf (_nss_ldap_filt_getgrent, LDAP_FILT_MAXSIZ, "(&(%s=%s))",
            attmap_objectClass, attmap_group_objectClass);
  snprintf (_nss_ldap_filt_getgroupsbymemberanddn, LDAP_FILT_MAXSIZ,
            "(&(%s=%s)(|(%s=%s)(%s=%s)))",
            attmap_objectClass, attmap_group_objectClass, attmap_group_memberUid, "%s", attmap_group_uniqueMember, "%s");
  snprintf (_nss_ldap_filt_getgroupsbydn, LDAP_FILT_MAXSIZ,
            "(&(%s=%s)(%s=%s))",
            attmap_objectClass, attmap_group_objectClass, attmap_group_uniqueMember, "%s");
  snprintf (_nss_ldap_filt_getpwnam_groupsbymember, LDAP_FILT_MAXSIZ,
            "(|(&(%s=%s)(%s=%s))(&(%s=%s)(%s=%s)))",
            attmap_objectClass, attmap_group_objectClass, attmap_group_memberUid, "%s",
            attmap_objectClass, attmap_passwd_objectClass, attmap_passwd_uid, "%s");
  snprintf (_nss_ldap_filt_getgroupsbymember, LDAP_FILT_MAXSIZ,
            "(&(%s=%s)(%s=%s))", attmap_objectClass, attmap_group_objectClass, attmap_group_memberUid,
            "%s");

  /* IP hosts */
  snprintf (_nss_ldap_filt_gethostbyname, LDAP_FILT_MAXSIZ,
            "(&(%s=%s)(%s=%s))", attmap_objectClass, attmap_host_objectClass, attmap_host_cn,
            "%s");
  snprintf (_nss_ldap_filt_gethostbyaddr, LDAP_FILT_MAXSIZ,
            "(&(%s=%s)(%s=%s))", attmap_objectClass, attmap_host_objectClass, attmap_host_ipHostNumber,
            "%s");
  snprintf (_nss_ldap_filt_gethostent, LDAP_FILT_MAXSIZ, "(%s=%s)",
            attmap_objectClass, attmap_host_objectClass);

  /* IP networks */
  snprintf (_nss_ldap_filt_getnetbyname, LDAP_FILT_MAXSIZ,
            "(&(%s=%s)(%s=%s))", attmap_objectClass, attmap_network_objectClass,
            attmap_network_cn, "%s");
  snprintf (_nss_ldap_filt_getnetbyaddr, LDAP_FILT_MAXSIZ,
            "(&(%s=%s)(%s=%s))", attmap_objectClass, attmap_network_objectClass,
            attmap_network_ipNetworkNumber, "%s");
  snprintf (_nss_ldap_filt_getnetent, LDAP_FILT_MAXSIZ, "(%s=%s)",
            attmap_objectClass, attmap_network_objectClass);

  /* IP protocols */
  snprintf (_nss_ldap_filt_getprotobyname, LDAP_FILT_MAXSIZ,
            "(&(%s=%s)(%s=%s))", attmap_objectClass, attmap_protocol_objectClass,
            attmap_protocol_cn, "%s");
  snprintf (_nss_ldap_filt_getprotobynumber, LDAP_FILT_MAXSIZ,
            "(&(%s=%s)(%s=%s))", attmap_objectClass, attmap_protocol_objectClass,
            attmap_protocol_ipProtocolNumber, "%d");
  snprintf (_nss_ldap_filt_getprotoent, LDAP_FILT_MAXSIZ, "(%s=%s)",
            attmap_objectClass, attmap_protocol_objectClass);

  /* users */
  snprintf (_nss_ldap_filt_getpwnam, LDAP_FILT_MAXSIZ,
            "(&(%s=%s)(%s=%s))", attmap_objectClass, attmap_passwd_objectClass,
            attmap_passwd_uid, "%s");
  snprintf (_nss_ldap_filt_getpwuid, LDAP_FILT_MAXSIZ,
            "(&(%s=%s)(%s=%s))",
            attmap_objectClass, attmap_passwd_objectClass, attmap_passwd_uidNumber, "%d");
  snprintf (_nss_ldap_filt_getpwent, LDAP_FILT_MAXSIZ,
            "(%s=%s)", attmap_objectClass, attmap_passwd_objectClass);

  /* RPCs */
  snprintf (_nss_ldap_filt_getrpcbyname, LDAP_FILT_MAXSIZ,
            "(&(%s=%s)(%s=%s))", attmap_objectClass, attmap_rpc_objectClass, attmap_rpc_cn, "%s");
  snprintf (_nss_ldap_filt_getrpcbynumber, LDAP_FILT_MAXSIZ,
            "(&(%s=%s)(%s=%s))", attmap_objectClass, attmap_rpc_objectClass, attmap_rpc_oncRpcNumber,
            "%d");
  snprintf (_nss_ldap_filt_getrpcent, LDAP_FILT_MAXSIZ, "(%s=%s)",
            attmap_objectClass, attmap_rpc_objectClass);

  /* IP services */
  snprintf (_nss_ldap_filt_getservbyname, LDAP_FILT_MAXSIZ,
            "(&(%s=%s)(%s=%s))", attmap_objectClass, attmap_service_objectClass, attmap_service_cn,
            "%s");
  snprintf (_nss_ldap_filt_getservbynameproto, LDAP_FILT_MAXSIZ,
            "(&(%s=%s)(%s=%s)(%s=%s))",
            attmap_objectClass, attmap_service_objectClass, attmap_service_cn, "%s", attmap_service_ipServiceProtocol,
            "%s");
  snprintf (_nss_ldap_filt_getservbyport, LDAP_FILT_MAXSIZ,
            "(&(%s=%s)(%s=%s))", attmap_objectClass, attmap_service_objectClass, attmap_service_ipServicePort,
            "%d");
  snprintf (_nss_ldap_filt_getservbyportproto, LDAP_FILT_MAXSIZ,
            "(&(%s=%s)(%s=%s)(%s=%s))", attmap_objectClass, attmap_service_objectClass,
            attmap_service_ipServicePort, "%d", attmap_service_ipServiceProtocol, "%s");
  snprintf (_nss_ldap_filt_getservent, LDAP_FILT_MAXSIZ, "(%s=%s)",
            attmap_objectClass, attmap_service_objectClass);

  /* shadow users */
  snprintf (_nss_ldap_filt_getspnam, LDAP_FILT_MAXSIZ,
            "(&(%s=%s)(%s=%s))", attmap_objectClass, attmap_shadow_objectClass,
            attmap_shadow_uid, "%s");
  snprintf (_nss_ldap_filt_getspent, LDAP_FILT_MAXSIZ,
            "(%s=%s)", attmap_objectClass, attmap_shadow_objectClass);

  /* netgroups */
  snprintf (_nss_ldap_filt_getnetgrent, LDAP_FILT_MAXSIZ,
            "(&(%s=%s)(%s=%s))", attmap_objectClass, attmap_netgroup_objectClass,
            attmap_netgroup_cn, "%s");

}

static void
init_pwd_attributes (const char ***pwd_attrs)
{
  int i = 0;
  static const char *__pwd_attrs[ATTRTAB_SIZE + 1];

  (*pwd_attrs) = __pwd_attrs;

  (*pwd_attrs)[i++] = attmap_passwd_uid;
  (*pwd_attrs)[i++] = attmap_passwd_userPassword;
  (*pwd_attrs)[i++] = attmap_passwd_uidNumber;
  (*pwd_attrs)[i++] = attmap_passwd_gidNumber;
  (*pwd_attrs)[i++] = attmap_passwd_cn;
  (*pwd_attrs)[i++] = attmap_passwd_homeDirectory;
  (*pwd_attrs)[i++] = attmap_passwd_loginShell;
  (*pwd_attrs)[i++] = attmap_passwd_gecos;
  (*pwd_attrs)[i++] = "description";
  (*pwd_attrs)[i++] = attmap_objectClass;
  (*pwd_attrs)[i] = NULL;
}

static void
init_sp_attributes (const char ***sp_attrs)
{
  static const char *__sp_attrs[ATTRTAB_SIZE + 1];

  (*sp_attrs) = __sp_attrs;

  (*sp_attrs)[0] = attmap_shadow_uid;
  (*sp_attrs)[1] = attmap_shadow_userPassword;
  (*sp_attrs)[2] = attmap_shadow_shadowLastChange;
  (*sp_attrs)[3] = attmap_shadow_shadowMax;
  (*sp_attrs)[4] = attmap_shadow_shadowMin;
  (*sp_attrs)[5] = attmap_shadow_shadowWarning;
  (*sp_attrs)[6] = attmap_shadow_shadowInactive;
  (*sp_attrs)[7] = attmap_shadow_shadowExpire;
  (*sp_attrs)[8] = attmap_shadow_shadowFlag;
  (*sp_attrs)[9] = NULL;
}

static void
init_grp_attributes (const char ***grp_attrs)
{
  int i = 0;
  static const char *__grp_attrs[ATTRTAB_SIZE + 1];

  (*grp_attrs) = __grp_attrs;

  (*grp_attrs)[i++] = attmap_group_cn;
  (*grp_attrs)[i++] = attmap_group_userPassword;
  (*grp_attrs)[i++] = attmap_group_memberUid;
  if (_nss_ldap_test_config_flag (NSS_LDAP_FLAGS_RFC2307BIS))
    (*grp_attrs)[i++] = attmap_group_uniqueMember;
  (*grp_attrs)[i++] = attmap_group_gidNumber;
  (*grp_attrs)[i] = NULL;
}

static void
init_hosts_attributes (const char ***hosts_attrs)
{
  static const char *__hosts_attrs[ATTRTAB_SIZE + 1];

  (*hosts_attrs) = __hosts_attrs;

  (*hosts_attrs)[0] = attmap_host_cn;
  (*hosts_attrs)[1] = attmap_host_ipHostNumber;
  (*hosts_attrs)[2] = NULL;
}

static void
init_services_attributes (const char ***services_attrs)
{
  static const char *__services_attrs[ATTRTAB_SIZE + 1];

  (*services_attrs) = __services_attrs;

  (*services_attrs)[0] = attmap_service_cn;
  (*services_attrs)[1] = attmap_service_ipServicePort;
  (*services_attrs)[2] = attmap_service_ipServiceProtocol;
  (*services_attrs)[3] = NULL;
}

static void
init_network_attributes (const char ***network_attrs)
{
  static const char *__network_attrs[ATTRTAB_SIZE + 1];

  (*network_attrs) = __network_attrs;

  (*network_attrs)[0] = attmap_network_cn;
  (*network_attrs)[1] = attmap_network_ipNetworkNumber;
  (*network_attrs)[2] = NULL;
}

static void
init_proto_attributes (const char ***proto_attrs)
{
  static const char *__proto_attrs[ATTRTAB_SIZE + 1];

  (*proto_attrs) = __proto_attrs;

  (*proto_attrs)[0] = attmap_protocol_cn;
  (*proto_attrs)[1] = attmap_protocol_ipProtocolNumber;
  (*proto_attrs)[2] = NULL;
}

static void
init_rpc_attributes (const char ***rpc_attrs)
{
  static const char *__rpc_attrs[ATTRTAB_SIZE + 1];

  (*rpc_attrs) = __rpc_attrs;

  (*rpc_attrs)[0] = attmap_rpc_cn;
  (*rpc_attrs)[1] = attmap_rpc_oncRpcNumber;
  (*rpc_attrs)[2] = NULL;
}

static void
init_ethers_attributes (const char ***ethers_attrs)
{
  static const char *__ethers_attrs[ATTRTAB_SIZE + 1];

  (*ethers_attrs) = __ethers_attrs;

  (*ethers_attrs)[0] = attmap_ether_cn;
  (*ethers_attrs)[1] = attmap_ether_macAddress;
  (*ethers_attrs)[2] = NULL;
}

static void
init_alias_attributes (const char ***alias_attrs)
{
  static const char *__alias_attrs[ATTRTAB_SIZE + 1];

  (*alias_attrs) = __alias_attrs;

  (*alias_attrs)[0] = attmap_alias_cn;
  (*alias_attrs)[1] = attmap_alias_rfc822MailMember;
  (*alias_attrs)[2] = NULL;
}

static void
init_netgrp_attributes (const char ***netgrp_attrs)
{
  static const char *__netgrp_attrs[ATTRTAB_SIZE + 1];

  (*netgrp_attrs) = __netgrp_attrs;

  (*netgrp_attrs)[0] = attmap_netgroup_cn;
  (*netgrp_attrs)[1] = attmap_netgroup_nisNetgroupTriple;
  (*netgrp_attrs)[2] = attmap_netgroup_memberNisNetgroup;
  (*netgrp_attrs)[3] = NULL;
}

/**
 * attribute table initialization routines
 */
void
_nss_ldap_init_attributes (const char ***attrtab)
{
  init_pwd_attributes (&attrtab[LM_PASSWD]);
  init_sp_attributes (&attrtab[LM_SHADOW]);
  init_grp_attributes (&attrtab[LM_GROUP]);
  init_hosts_attributes (&attrtab[LM_HOSTS]);
  init_services_attributes (&attrtab[LM_SERVICES]);
  init_network_attributes (&attrtab[LM_NETWORKS]);
  init_proto_attributes (&attrtab[LM_PROTOCOLS]);
  init_rpc_attributes (&attrtab[LM_RPC]);
  init_ethers_attributes (&attrtab[LM_ETHERS]);
  init_network_attributes (&attrtab[LM_NETMASKS]);
  init_alias_attributes (&attrtab[LM_ALIASES]);
  init_netgrp_attributes (&attrtab[LM_NETGROUP]);

  attrtab[LM_NONE] = NULL;
}


